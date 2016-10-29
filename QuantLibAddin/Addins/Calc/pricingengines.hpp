
/*  
 Copyright (C) 2006, 2007, 2008, 2012 Ferdinando Ametrano
 Copyright (C) 2007 Eric Ehlers
 Copyright (C) 2016 Stefano Fondi
 
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

// This file was generated automatically by gensrc.py.  If you edit this file
// manually then your changes will be lost the next time gensrc runs.

// This source code file was generated from the following stub:
//      gensrc/gensrc/stubs/stub.calc.category

#ifndef ql_calc_pricingengines_hpp
#define ql_calc_pricingengines_hpp

    SEQSEQ(ANY) SAL_CALL qlAnalyticCapFloorEngine(
        const ANY &ObjectId,
        const ANY &HandleModel,
        const sal_Int32 Permanent,
        const ANY &Trigger,
        const sal_Int32 Overwrite) throw(RuntimeException);

    SEQSEQ(ANY) SAL_CALL qlBachelierCapFloorEngine(
        const ANY &ObjectId,
        const ANY &YieldCurve,
        const ANY &VolTS,
        const sal_Int32 Permanent,
        const ANY &Trigger,
        const sal_Int32 Overwrite) throw(RuntimeException);

    SEQSEQ(ANY) SAL_CALL qlBachelierCapFloorEngine2(
        const ANY &ObjectId,
        const ANY &YieldCurve,
        const ANY &Vol,
        const ANY &DayCounter,
        const sal_Int32 Permanent,
        const ANY &Trigger,
        const sal_Int32 Overwrite) throw(RuntimeException);

    SEQSEQ(ANY) SAL_CALL qlBinomialPricingEngine(
        const ANY &ObjectId,
        const ANY &EngineID,
        const ANY &ProcessID,
        const ANY &TimeSteps,
        const sal_Int32 Permanent,
        const ANY &Trigger,
        const sal_Int32 Overwrite) throw(RuntimeException);

    SEQSEQ(ANY) SAL_CALL qlBlackCapFloorEngine(
        const ANY &ObjectId,
        const ANY &YieldCurve,
        const ANY &VolTS,
        const ANY &Displacement,
        const sal_Int32 Permanent,
        const ANY &Trigger,
        const sal_Int32 Overwrite) throw(RuntimeException);

    SEQSEQ(ANY) SAL_CALL qlBlackCapFloorEngine2(
        const ANY &ObjectId,
        const ANY &YieldCurve,
        const ANY &Vol,
        const ANY &Displacement,
        const ANY &DayCounter,
        const sal_Int32 Permanent,
        const ANY &Trigger,
        const sal_Int32 Overwrite) throw(RuntimeException);

    SEQSEQ(ANY) SAL_CALL qlBlackSwaptionEngine(
        const ANY &ObjectId,
        const ANY &YieldCurve,
        const ANY &VolTS,
        const ANY &Displacement,
        const sal_Int32 Permanent,
        const ANY &Trigger,
        const sal_Int32 Overwrite) throw(RuntimeException);

    SEQSEQ(ANY) SAL_CALL qlBlackSwaptionEngine2(
        const ANY &ObjectId,
        const ANY &YieldCurve,
        const ANY &Vol,
        const ANY &Displacement,
        const ANY &DayCounter,
        const sal_Int32 Permanent,
        const ANY &Trigger,
        const sal_Int32 Overwrite) throw(RuntimeException);

    SEQSEQ(ANY) SAL_CALL qlBondEngine(
        const ANY &ObjectId,
        const ANY &YieldCurve,
        const sal_Int32 Permanent,
        const ANY &Trigger,
        const sal_Int32 Overwrite) throw(RuntimeException);

    SEQSEQ(ANY) SAL_CALL qlDiscountingSwapEngine(
        const ANY &ObjectId,
        const ANY &YieldCurve,
        const sal_Int32 IncludeSettlDate,
        const ANY &SettlementDate,
        const ANY &NpvDate,
        const sal_Int32 Permanent,
        const ANY &Trigger,
        const sal_Int32 Overwrite) throw(RuntimeException);

    SEQSEQ(ANY) SAL_CALL qlJamshidianSwaptionEngine(
        const ANY &ObjectId,
        const ANY &Model,
        const ANY &YieldCurve,
        const sal_Int32 Permanent,
        const ANY &Trigger,
        const sal_Int32 Overwrite) throw(RuntimeException);

    SEQSEQ(ANY) SAL_CALL qlModelG2SwaptionEngine(
        const ANY &ObjectId,
        const ANY &Model,
        const ANY &Range,
        const ANY &Intervals,
        const sal_Int32 Permanent,
        const ANY &Trigger,
        const sal_Int32 Overwrite) throw(RuntimeException);

    SEQSEQ(ANY) SAL_CALL qlPricingEngine(
        const ANY &ObjectId,
        const ANY &EngineID,
        const ANY &ProcessID,
        const sal_Int32 Permanent,
        const ANY &Trigger,
        const sal_Int32 Overwrite) throw(RuntimeException);

    SEQSEQ(ANY) SAL_CALL qlTreeSwaptionEngine(
        const ANY &ObjectId,
        const ANY &Model,
        const ANY &Nsteps,
        const ANY &YieldCurve,
        const sal_Int32 Permanent,
        const ANY &Trigger,
        const sal_Int32 Overwrite) throw(RuntimeException);



#endif

