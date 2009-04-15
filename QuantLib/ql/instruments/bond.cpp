/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2004 Jeff Yu
 Copyright (C) 2004 M-Dimension Consulting Inc.
 Copyright (C) 2005, 2006, 2007, 2008 StatPro Italia srl
 Copyright (C) 2007, 2008, 2009 Ferdinando Ametrano
 Copyright (C) 2007 Chiara Fornarola
 Copyright (C) 2008 Simon Ibbotson

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#include <ql/instruments/bond.hpp>
#include <ql/cashflows/cashflows.hpp>
#include <ql/cashflows/coupon.hpp>
#include <ql/math/solvers1d/brent.hpp>
#include <ql/cashflows/simplecashflow.hpp>
#include <ql/pricingengines/bond/discountingbondengine.hpp>
#include <ql/pricingengines/bond/bondfunctions.hpp>

using boost::shared_ptr;
using boost::dynamic_pointer_cast;

namespace QuantLib {

    Bond::Bond(Natural settlementDays,
               const Calendar& calendar,
               const Date& issueDate,
               const Leg& coupons)
      : settlementDays_(settlementDays), calendar_(calendar),
        cashflows_(coupons), issueDate_(issueDate) {

        if (!coupons.empty()) {
            std::sort(cashflows_.begin(), cashflows_.end(),
                      earlier_than<boost::shared_ptr<CashFlow> >());

            maturityDate_ = coupons.back()->date();

            addRedemptionsToCashflows();
        }

        registerWith(Settings::instance().evaluationDate());
    }

    Bond::Bond(Natural settlementDays,
               const Calendar& calendar,
               Real faceAmount,
               const Date& maturityDate,
               const Date& issueDate,
               const Leg& cashflows)
    : settlementDays_(settlementDays), calendar_(calendar),
      cashflows_(cashflows), maturityDate_(maturityDate),
      issueDate_(issueDate) {

        if (!cashflows.empty()) {

            notionals_.resize(2);
            notionalSchedule_.resize(2);

            notionalSchedule_[0] = Date();
            notionals_[0] = faceAmount;

            notionalSchedule_[1] = maturityDate;
            notionals_[1] = 0.0;

            redemptions_.push_back(cashflows.back());

            std::sort(cashflows_.begin(), cashflows_.end()-1,
                      earlier_than<boost::shared_ptr<CashFlow> >());
        }

        registerWith(Settings::instance().evaluationDate());
    }

    Real Bond::notional(Date d) const {
        if (d == Date())
            d = settlementDate();

        if (d > notionalSchedule_.back()) {
            // after maturity
            return 0.0;
        }

        // After the check above, d is between the schedule
        // boundaries.  We search starting from the second notional
        // date, since the first is null.  After the call to
        // lower_bound, *i is the earliest date which is greater or
        // equal than d.  Its index is greater or equal to 1.
        std::vector<Date>::const_iterator i =
            std::lower_bound(notionalSchedule_.begin()+1,
                             notionalSchedule_.end(), d);
        Size index = std::distance(notionalSchedule_.begin(), i);

        if (d < notionalSchedule_[index]) {
            // no doubt about what to return
            return notionals_[index-1];
        } else {
            // d is equal to a redemption date.
            #if defined(QL_TODAYS_PAYMENTS)
            // We consider today's payment as pending; the bond still
            // has the previous notional
            return notionals_[index-1];
            #else
            // today's payment has occurred; the bond already changed
            // notional.
            return notionals_[index];
            #endif
        }
    }

    const boost::shared_ptr<CashFlow>& Bond::redemption() const {
        QL_REQUIRE(redemptions_.size() == 1,
                   "multiple redemption cash flows given");
        return redemptions_.back();
    }

    Date Bond::maturityDate() const {
        if (maturityDate_!=Null<Date>())
            return maturityDate_;
        else
            return BondFunctions::maturityDate(*this);
    }

    Date Bond::settlementDate(const Date& date) const {
        Date d = (date==Date() ?
                  Settings::instance().evaluationDate() :
                  date);

        // usually, the settlement is at T+n...
        Date settlement = calendar_.advance(d, settlementDays_, Days);
        // ...but the bond won't be traded until the issue date (if given.)
        if (issueDate_ == Date())
            return settlement;
        else
            return std::max(settlement, issueDate_);
    }

    Real Bond::cleanPrice() const {
        return dirtyPrice() - accruedAmount(settlementDate());
    }

    Real Bond::dirtyPrice() const {
        return settlementValue() * 100.0 / notional(settlementDate());
    }

    Real Bond::settlementValue() const {
        calculate();
        QL_REQUIRE(settlementValue_ != Null<Real>(),
                   "settlement value not provided");
        return settlementValue_;
    }

    Real Bond::settlementValue(Real cleanPrice) const {
        Real dirtyPrice = cleanPrice + accruedAmount(settlementDate());
        return dirtyPrice / 100.0 * notional(settlementDate());
    }

    Rate Bond::yield(const DayCounter& dc,
                     Compounding comp,
                     Frequency freq,
                     Real accuracy,
                     Size maxEvaluations) const {
        return BondFunctions::yield(*this, cleanPrice(), dc, comp, freq,
                                    settlementDate(),
                                    accuracy, maxEvaluations);
    }

    Real Bond::cleanPrice(Rate y,
                          const DayCounter& dc,
                          Compounding comp,
                          Frequency freq,
                          Date settlement) const {
        return BondFunctions::cleanPrice(*this, y, dc, comp, freq, settlement);
    }

    Real Bond::dirtyPrice(Rate y,
                          const DayCounter& dc,
                          Compounding comp,
                          Frequency freq,
                          Date settlement) const {
        return BondFunctions::cleanPrice(*this, y, dc, comp, freq, settlement)
            + accruedAmount(settlement);
    }

    Rate Bond::yield(Real cleanPrice,
                     const DayCounter& dc,
                     Compounding comp,
                     Frequency freq,
                     Date settlement,
                     Real accuracy,
                     Size maxEvaluations) const {
        return BondFunctions::yield(*this, cleanPrice, dc, comp, freq,
                                    settlement, accuracy, maxEvaluations);
    }

    Real Bond::accruedAmount(Date settlement) const {
        return BondFunctions::accruedAmount(*this, settlement);
    }

    bool Bond::isExpired() const {
        return CashFlows::isExpired(cashflows_, settlementDate());
    }

    Rate Bond::nextCouponRate(Date settlement) const {
        return BondFunctions::nextCouponRate(*this, settlement);
    }

    Rate Bond::previousCouponRate(Date settlement) const {
        return BondFunctions::previousCouponRate(*this, settlement);
    }

    Date Bond::nextCashFlowDate(Date settlement) const {
        if (settlement == Date())
            settlement = settlementDate();
        return CashFlows::nextCashFlowDate(cashflows_, settlement);
    }

    Date Bond::previousCashFlowDate(Date settlement) const {
        if (settlement == Date())
            settlement = settlementDate();
        return CashFlows::previousCashFlowDate(cashflows_, settlement);
    }

    void Bond::setupExpired() const {
        Instrument::setupExpired();
        settlementValue_ = 0.0;
    }

    void Bond::setupArguments(PricingEngine::arguments* args) const {
        Bond::arguments* arguments = dynamic_cast<Bond::arguments*>(args);
        QL_REQUIRE(arguments != 0, "wrong argument type");

        arguments->settlementDate = settlementDate();
        arguments->cashflows = cashflows_;
        arguments->calendar = calendar_;
    }

    void Bond::fetchResults(const PricingEngine::results* r) const {

        Instrument::fetchResults(r);

        const Bond::results* results =
            dynamic_cast<const Bond::results*>(r);
        QL_ENSURE(results != 0, "wrong result type");

        settlementValue_ = results->settlementValue;
    }

    void Bond::addRedemptionsToCashflows(const std::vector<Real>& redemptions) {
        // First, we gather the notional information from the cashflows
        calculateNotionalsFromCashflows();
        // Then, we create the redemptions based on the notional
        // information and we add them to the cashflows vector after
        // the coupons.
        redemptions_.clear();
        for (Size i=1; i<notionalSchedule_.size(); ++i) {
            Real R = i < redemptions.size() ? redemptions[i] :
                     !redemptions.empty()   ? redemptions.back() :
                                              100.0;
            Real amount = (R/100.0)*(notionals_[i-1]-notionals_[i]);
            boost::shared_ptr<CashFlow> redemption(
                            new SimpleCashFlow(amount, notionalSchedule_[i]));
            cashflows_.push_back(redemption);
            redemptions_.push_back(redemption);
        }
        // stable_sort now moves the redemptions to the right places
        // while ensuring that they follow coupons with the same date.
        std::stable_sort(cashflows_.begin(), cashflows_.end(),
                         earlier_than<boost::shared_ptr<CashFlow> >());
    }

    void Bond::setSingleRedemption(Real notional,
                                   Real redemption,
                                   const Date& date) {

        boost::shared_ptr<CashFlow> redemptionCashflow(
                         new SimpleCashFlow(notional*redemption/100.0, date));
        setSingleRedemption(notional, redemptionCashflow);
    }

    void Bond::setSingleRedemption(
                              Real notional,
                              const boost::shared_ptr<CashFlow>& redemption) {
        notionals_.resize(2);
        notionalSchedule_.resize(2);
        redemptions_.clear();

        notionalSchedule_[0] = Date();
        notionals_[0] = notional;

        notionalSchedule_[1] = redemption->date();
        notionals_[1] = 0.0;

        cashflows_.push_back(redemption);
        redemptions_.push_back(redemption);
    }


    void Bond::calculateNotionalsFromCashflows() {
        notionalSchedule_.clear();
        notionals_.clear();

        Date lastPaymentDate = Date();
        notionalSchedule_.push_back(Date());
        for (Size i=0; i<cashflows_.size(); ++i) {
            boost::shared_ptr<Coupon> coupon =
                boost::dynamic_pointer_cast<Coupon>(cashflows_[i]);
            if (!coupon)
                continue;

            Real notional = coupon->nominal();
            // we add the notional only if it is the first one...
            if (notionals_.empty()) {
                notionals_.push_back(coupon->nominal());
                lastPaymentDate = coupon->date();
            } else if (!close(notional, notionals_.back())) {
                // ...or if it has changed.
                QL_REQUIRE(notional < notionals_.back(),
                           "increasing coupon notionals");
                notionals_.push_back(coupon->nominal());
                // in this case, we also add the last valid date for
                // the previous one...
                notionalSchedule_.push_back(lastPaymentDate);
                // ...and store the candidate for this one.
                lastPaymentDate = coupon->date();
            } else {
                // otherwise, we just extend the valid range of dates
                // for the current notional.
                lastPaymentDate = coupon->date();
            }
        }
        QL_REQUIRE(!notionals_.empty(), "no coupons provided");
        notionals_.push_back(0.0);
        notionalSchedule_.push_back(lastPaymentDate);
    }


    void Bond::arguments::validate() const {
        QL_REQUIRE(settlementDate != Date(), "no settlement date provided");
        QL_REQUIRE(!cashflows.empty(), "no cash flow provided");
        for (Size i=0; i<cashflows.size(); ++i)
            QL_REQUIRE(cashflows[i], "null cash flow provided");
    }

}
