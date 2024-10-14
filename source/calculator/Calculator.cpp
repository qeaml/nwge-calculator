#include "Calculator.hpp"
#include "UI.hpp"

class Calculator final: public UI {
public:
  Calculator()
    : UI({
      .anchor = cAnchor,
      .margin = cMargin,
      .padding = cPadding,
      .textH = cTextH,
    })
  {
    display(cDisplaySize, [this](auto &cursor){ drawDisplay(cursor); });
    nextLine();
    button("*"_sv, cButtonSize, [this]{multiply();});
    button("/"_sv, cButtonSize, [this]{divide();});
    button("+"_sv, cButtonSize, [this]{add();});
    button("-"_sv, cButtonSize, [this]{subtract();});
    nextLine();
    button("7"_sv, cButtonSize, [this]{put7();});
    button("8"_sv, cButtonSize, [this]{put8();});
    button("9"_sv, cButtonSize, [this]{put9();});
    button("M+"_sv, cButtonSize, [this]{memAdd();});
    nextLine();
    button("4"_sv, cButtonSize, [this]{put4();});
    button("5"_sv, cButtonSize, [this]{put5();});
    button("6"_sv, cButtonSize, [this]{put6();});
    button("MR"_sv, cButtonSize, [this]{memRecall();});
    nextLine();
    button("1"_sv, cButtonSize, [this]{put1();});
    button("2"_sv, cButtonSize, [this]{put2();});
    button("3"_sv, cButtonSize, [this]{put3();});
    button("MC"_sv, cButtonSize, [this]{memClear();});
    nextLine();
    button("0"_sv, cButtonSize, [this]{put0();});
    button("."_sv, cButtonSize, [this]{putDecimal();});
    button("="_sv, cButtonSize, [this]{equals();});
    button("C"_sv, cButtonSize, [this]{clear();});
  }

private:
  static constexpr f32
    cMargin = 0.01f,
    cPadding = cMargin,
    cTextH = 0.08f;
  static constexpr glm::vec2
    cButtonSize{0.2f, 0.15f},
    cDisplaySize{4*cButtonSize.x + 3*cMargin, cButtonSize.y},
    cAnchor{
      (1.0f - 4*(cButtonSize.x+cMargin) - cMargin)/2.0f,
      (1.0f - 6*(cButtonSize.y+cMargin) - cMargin)/2.0f,
    };

  s64 mNumIntA = 0;
  s64 mNumDecA = 0;
  bool mNumHasDecA = false;
  char mOp = '\0';
  char mPrevOp = '\0';
  s64 mNumIntB = 0;
  s64 mNumDecB = 0;
  bool mNumHasDecB = false;
  nwge::ScratchString mDisplayStr;
  nwge::StringView mDisplayStrView;

  static f64 intDecToDouble(s64 intPart, s64 decPart) {
    f64 decVal = 0;
    if(decPart != 0) {
      f64 decDigits = 1;
      if(decPart > 1) {
        decDigits = ceil(log10(decPart));
      }
      f64 decDivisor = pow(10, decDigits);
      decVal = f64(decPart) / decDivisor;
    }
    return f64(intPart) + decVal;
  }

  static void doubleToIntDec(f64 val, s64 &intPart, s64 &decPart) {
    f64 integer;
    f64 decimal = modf(val, &integer);
    intPart = s64(integer);
    decPart = s64(round(decimal * 1000.0));
  }

  void drawDisplay(nwge::render::Cursor &cursor) const {
    cursor << s32(mNumIntA);
    if(mOp == '\0' && !mNumHasDecA) {
      return;
    }
    cursor << '.';
    cursor << s32(mNumDecA);
    if(mOp == '\0') {
      return;
    }
    cursor << ' ' << mOp << ' ';
    cursor << s32(mNumIntB);
    if(!mNumHasDecB) {
      return;
    }
    cursor << '.';
    cursor << s32(mNumDecB);
  }

  void putDigit(s64 digit) {
    if(mOp == '\0') {
      if(mNumHasDecA) {
        mNumDecA = mNumDecA * 10 + digit;
      } else {
        mNumIntA = mNumIntA * 10 + digit;
      }
    } else {
      if(mNumHasDecB) {
        mNumDecB = mNumDecB * 10 + digit;
      } else {
        mNumIntB = mNumIntB * 10 + digit;
      }
    }
  }

  void setOp(char oper) {
    mOp = oper;
  }

  s64 mMemInt = 0;
  s64 mMemDec = 0;

  void multiply() { setOp('*'); }
  void divide() { setOp('/'); }
  void add() { setOp('+'); }
  void subtract() { setOp('-'); }

  void put7() { putDigit(7); }
  void put8() { putDigit(8); }
  void put9() { putDigit(9); }
  void memAdd() {
    f64 mem = intDecToDouble(mMemInt, mMemDec);
    f64 val = intDecToDouble(mNumIntA, mNumDecA);
    doubleToIntDec(mem + val, mMemInt, mMemDec);
  }

  void put4() { putDigit(4); }
  void put5() { putDigit(5); }
  void put6() { putDigit(6); }
  void memRecall() {
    if(mOp == '\0') {
      mNumIntA = mMemInt;
      mNumDecA = mMemDec;
      mNumHasDecA = mNumDecA != 0;
    } else {
      mNumIntB = mMemInt;
      mNumDecB = mMemDec;
      mNumHasDecB = mNumDecB != 0;
    }
  }

  void put1() { putDigit(1); }
  void put2() { putDigit(2); }
  void put3() { putDigit(3); }
  void memClear() { mMemInt = 0; mMemDec = 0; }

  void put0() { putDigit(0); }
  void putDecimal() {
    if(mOp == '\0') {
      mNumHasDecA = true;
    } else {
      mNumHasDecB = true;
    }
  }
  void equals() {
    if(mOp == '\0') {
      if(mPrevOp == '\0') {
        return;
      }
      mOp = mPrevOp;
      mNumIntB = mNumIntA;
      mNumDecB = mNumDecA;
    }

    f64 numA = intDecToDouble(mNumIntA, mNumDecA);
    f64 numB = intDecToDouble(mNumIntB, mNumDecB);
    f64 result = 0;
    switch(mOp) {
    case '+': result = numA + numB; break;
    case '-': result = numA - numB; break;
    case '*': result = numA * numB; break;
    case '/': result = numA / numB; break;
    default: break;
    }
    doubleToIntDec(result, mNumIntA, mNumDecA);
    mNumHasDecA = mNumDecA != 0;
    mNumIntB = 0;
    mNumDecB = 0;
    mNumHasDecB = false;
    mPrevOp = mOp;
    mOp = '\0';
  }
  void clear() {
    mNumIntA = 0;
    mNumDecA = 0;
    mNumHasDecA = false;
    if(mOp != '\0') {
      mNumIntB = 0;
      mNumDecB = 0;
      mNumHasDecB = false;
      mOp = '\0';
    }
  }
};

nwge::State *createCalculator() {
  return new Calculator();
}
