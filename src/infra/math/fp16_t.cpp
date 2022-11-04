/**
 * Copyright 2019-2022 Huawei Technologies Co., Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "infra/math/fp16_t.h"

namespace ge {
/*
 * @ingroup fp16_t global filed
 * @brief   round mode of last valid digital
 */
fp16RoundMode_t g_RoundMode = ROUND_TO_NEAREST;

void ExtractFP16(const uint16_t& val, uint16_t* s, int16_t* e, uint16_t* m)
{
    // 1.Extract
    *s = FP16_EXTRAC_SIGN(val);
    *e = FP16_EXTRAC_EXP(val);
    *m = FP16_EXTRAC_MAN(val);

    // Denormal
    if ((*e) == 0) {
        *e = 1;
    }
}

/*
 * @ingroup fp16_t static method
 * @param [in] man       truncated mantissa
 * @param [in] shift_out left shift bits based on ten bits
 * @brief   judge whether to add one to the result while converting fp16_t to other datatype
 * @return  Return true if add one, otherwise false
 */
static bool IsRoundOne(uint64_t man, uint16_t truncLen)
{
    uint64_t mask0 = 0x4;
    uint64_t mask1 = 0x2;
    uint64_t mask2;
    uint16_t shiftOut = truncLen - 2;
    mask0 = mask0 << shiftOut;
    mask1 = mask1 << shiftOut;
    mask2 = mask1 - 1;

    bool lastBit = ((man & mask0) > 0);
    bool truncHigh = false;
    bool truncLeft = false;
    if (g_RoundMode == ROUND_TO_NEAREST) {
        truncHigh = ((man & mask1) > 0);
        truncLeft = ((man & mask2) > 0);
    }
    return (truncHigh && (truncLeft || lastBit));
}

/*
 * @ingroup fp16_t public method
 * @param [in] exp       exponent of fp16_t value
 * @param [in] man       exponent of fp16_t value
 * @brief   normalize fp16_t value
 * @return
 */
static void Fp16Normalize(int16_t& exp, uint16_t& man)
{
    if (exp >= FP16_MAX_EXP) {
        exp = FP16_MAX_EXP - 1;
        man = FP16_MAX_MAN;
    } else if (exp == 0 && man == FP16_MAN_HIDE_BIT) {
        exp++;
        man = 0;
    }
}

/*
 * @ingroup fp16_t math conversion static method
 * @param [in] fpVal uint16_t value of fp16_t object
 * @brief   Convert fp16_t to float/fp32
 * @return  Return float/fp32 value of fpVal which is the value of fp16_t object
 */
static float fp16ToFloat(const uint16_t& fpVal)
{
    float ret;

    uint16_t hfSign, hfMan;
    int16_t hfExp;
    ExtractFP16(fpVal, &hfSign, &hfExp, &hfMan);

    while (hfMan && !(hfMan & FP16_MAN_HIDE_BIT)) {
        hfMan <<= 1;
        hfExp--;
    }

    uint32_t sRet, eRet, mRet, fVal;

    sRet = hfSign;
    if (!hfMan) {
        eRet = 0;
        mRet = 0;
    } else {
        eRet = static_cast<uint32_t>(hfExp - FP16_EXP_BIAS + FP32_EXP_BIAS);
        mRet = hfMan & FP16_MAN_MASK;
        mRet = mRet << (FP32_MAN_LEN - FP16_MAN_LEN);
    }
    fVal = FP32_CONSTRUCTOR(sRet, eRet, mRet);
    ret = *(reinterpret_cast<float*>(&fVal));

    return ret;
}

/*
 * @ingroup fp16_t math conversion static method
 * @param [in] fpVal uint16_t value of fp16_t object
 * @brief   Convert fp16_t to uint8_t
 * @return  Return uint8_t value of fpVal which is the value of fp16_t object
 */
static uint8_t fp16ToUInt8(const uint16_t& fpVal)
{
    uint8_t ret;
    uint8_t mRet = 0;
    uint16_t hfE, hfM;

    // 1.get s_ret and shift it to bit0.
    uint8_t sRet = FP16_EXTRAC_SIGN(fpVal);
    // 2.get hf_e and hf_m
    hfE = FP16_EXTRAC_EXP(fpVal);
    hfM = FP16_EXTRAC_MAN(fpVal);

    if (FP16_IS_DENORM(fpVal) || sRet == 1) { // Denormalized number
        return 0;
    }

    if (FP16_IS_INVALID(fpVal)) { // Inf or NaN
        mRet = ~0;
        return mRet;
    }

    uint64_t longIntM = hfM;
    uint8_t overflowFlag = 0;
    uint16_t shiftOut = 0;

    while (hfE != FP16_EXP_BIAS) {
        if (hfE > FP16_EXP_BIAS) {
            hfE--;
            longIntM = longIntM << 1;
            if (longIntM >= 0x40000Lu) { // overflow 0100 0000 0000 0000 0000
                longIntM = 0x3FFFFLu; // 11 1111 1111 1111 1111   10(fp16_t-man)+8(uint8)=18bit
                overflowFlag = 1;
                mRet = ~0;
                break;
            }
        } else {
            hfE++;
            shiftOut++;
        }
    }
    if (!overflowFlag) {
        mRet = static_cast<uint8_t>(((longIntM >> (FP16_MAN_LEN + shiftOut)) & BIT_LEN8_MAX));
    }
    // Generate final result
    ret = mRet;

    return ret;
}

void fp16AddmRet(uint16_t& mRet, int16_t& eRet, uint32_t& mTrunc, const uint16_t& mMin, const uint16_t& mMax)
{
    while (mRet < mMin && eRet > 0) { // the value of m_ret should not be smaller than 2^23
        mRet = mRet << 1;
        mRet += (FP32_SIGN_MASK & mTrunc) >> FP32_SIGN_INDEX;
        mTrunc = mTrunc << 1;
        eRet = eRet - 1;
    }
    while (mRet >= mMax) { // the value of m_ret should be smaller than 2^24
        mTrunc = mTrunc >> 1;
        mTrunc = mTrunc | (FP32_SIGN_MASK * (mRet & 1));
        mRet = mRet >> 1;
        eRet = eRet + 1;
    }
}

// evaluation
__attribute__((__visibility__("default"))) fp16_t& fp16_t::operator=(const fp16_t& fp)
{
    if (&fp == this) {
        return *this;
    }
    val = fp.val;
    return *this;
}

__attribute__((__visibility__("default"))) fp16_t& fp16_t::operator=(const float& fVal)
{
    uint16_t sRet, mRet;
    int16_t eRet;
    uint32_t eF, mF;
    uint32_t ui32V = *(reinterpret_cast<uint32_t*>(const_cast<float*>(&fVal))); // 1:8:23bit sign:exp:man
    uint32_t mLenDelta;

    sRet = static_cast<uint16_t>((ui32V & FP32_SIGN_MASK) >> FP32_SIGN_INDEX); // 4Byte->2Byte
    eF = (ui32V & FP32_EXP_MASK) >> FP32_MAN_LEN; // 8 bit exponent
    mF = (ui32V & FP32_MAN_MASK); // 23 bit mantissa dont't need to care about denormal
    mLenDelta = FP32_MAN_LEN - FP16_MAN_LEN;

    bool needRound = false;
    // Exponent overflow/NaN converts to signed inf/NaN
    if (eF > 0x8Fu) { // 0x8Fu:142=127+15
        eRet = FP16_MAX_EXP - 1;
        mRet = FP16_MAX_MAN;
    } else if (eF <= 0x70u) { // 0x70u:112=127-15 Exponent underflow converts to denormalized half or signed zero
        eRet = 0;
        if (eF >= 0x67) { // 0x67:103=127-24 Denormal
            mF = (mF | FP32_MAN_HIDE_BIT);
            uint16_t shiftOut = FP32_MAN_LEN;
            uint64_t mTmp = (static_cast<uint64_t>(mF)) << (eF - 0x67);

            needRound = IsRoundOne(mTmp, shiftOut);
            mRet = static_cast<uint16_t>(mTmp >> shiftOut);
            if (needRound) {
                mRet++;
            }
        } else if (eF == 0x66 && mF > 0) { // 0x66:102 Denormal 0<f_v<min(Denormal)
            mRet = 1;
        } else {
            mRet = 0;
        }
    } else { // Regular case with no overflow or underflow
        eRet = static_cast<int16_t>(eF - 0x70u);

        needRound = IsRoundOne(mF, mLenDelta);
        mRet = static_cast<uint16_t>(mF >> mLenDelta);
        if (needRound) {
            mRet++;
        }
        if ((mRet & FP16_MAN_HIDE_BIT) != 0) {
            eRet++;
        }
    }

    Fp16Normalize(eRet, mRet);
    val = FP16_CONSTRUCTOR(sRet, static_cast<uint16_t>(eRet), mRet);
    return *this;
}

// convert
__attribute__((__visibility__("default"))) fp16_t::operator float() const
{
    return fp16ToFloat(val);
}

__attribute__((__visibility__("default"))) float fp16_t::toFloat()
{
    return fp16ToFloat(val);
}

__attribute__((__visibility__("default"))) uint8_t fp16_t::toUInt8()
{
    return fp16ToUInt8(val);
}
} /* namespace ge */
