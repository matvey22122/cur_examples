#ifndef biginteger_h
#define biginteger_h

#include <iostream>
#include <vector>
#include <string>
#include <math.h>
#include <complex>

class BigInteger {
 public:
  BigInteger();
  BigInteger(int64_t);
  BigInteger(const BigInteger&);
  BigInteger& operator=(const BigInteger&);
  BigInteger& operator+=(const BigInteger&);
  BigInteger& operator-=(const BigInteger&);
  BigInteger& operator*=(const BigInteger&);
  BigInteger& operator/=(const BigInteger&);
  BigInteger& operator%=(const BigInteger&);
  BigInteger operator-() const;
  BigInteger& operator++();
  BigInteger operator++(int);
  BigInteger& operator--();
  BigInteger operator--(int);
  explicit operator bool() const {
    return !(Size() == 1 && digits_[0] == 0);
  }
  std::string toString() const;
  friend bool operator<(const BigInteger&, const BigInteger&);
  friend std::ostream& operator<<(std::ostream&, const BigInteger&);
  friend std::istream& operator>>(std::istream&, BigInteger&);
  friend BigInteger operator""_bi(unsigned long long);
  friend BigInteger operator""_bi(const char*, size_t);
  BigInteger modulo() const;
  int Size() const;
 private:
  enum class Sign {
    MINUS = -1,
    PLUS = 1
  };
  std::vector<int64_t> digits_;
  Sign sign_ = Sign::PLUS;
  static const int64_t kBase = 1e9;
  bool moduloCmp(const BigInteger&, const BigInteger&);
  BigInteger subtract(BigInteger bi1, const BigInteger& bi2) {
    long long remainder = 0;
    for (int i = 0; i < bi2.Size() || remainder; ++i) {
      int64_t minus = remainder + (i < bi2.Size() ? bi2.digits_[i] : 0);
      remainder = 0;
      if (minus > static_cast<int64_t>(bi1.digits_[i])) {
        remainder = 1;
        bi1.digits_[i] += kBase;
      }
      bi1.digits_[i] -= minus;
    }
    while (bi1.Size() > 1 && bi1.digits_.back() == 0) {
      bi1.digits_.pop_back();
    }
    
    return bi1;
  }
  BigInteger sum(BigInteger bi1, const BigInteger& bi2) {
    int64_t remainder = 0;
    for (int i = 0; i < bi2.Size() || remainder; ++i) {
      if (i == bi1.Size()) {
        bi1.digits_.push_back(0);
      }
      bi1.digits_[i] += remainder + (i < bi2.Size() ? bi2.digits_[i] : 0);
      remainder = bi1.digits_[i] >= kBase;
      if (remainder) {
        bi1.digits_[i] -= kBase;
      }
    }
    return bi1;
  }
  void swap(BigInteger&, BigInteger&);
  BigInteger divByInt(int64_t);
  BigInteger multByInt(int64_t);
  void norm();
};

bool operator==(const BigInteger& bi1, const BigInteger& bi2);

bool operator!=(const BigInteger& bi1, const BigInteger& bi2);

bool operator<(const BigInteger& bi1, const BigInteger& bi2);

bool operator>(const BigInteger& bi1, const BigInteger& bi2);

bool operator<=(const BigInteger& bi1, const BigInteger& bi2);

bool operator>=(const BigInteger& bi1, const BigInteger& bi2);

BigInteger operator+(const BigInteger& bi1, const BigInteger& bi2);

BigInteger operator-(const BigInteger& bi1, const BigInteger& bi2);

BigInteger operator*(const BigInteger& bi1, const BigInteger& bi2);

BigInteger operator/(const BigInteger& bi1, const BigInteger& bi2);

BigInteger operator%(const BigInteger& bi1, const BigInteger& bi2);

BigInteger::BigInteger() : digits_({0}) {}

BigInteger::BigInteger(int64_t num) {
  if (num < 0) {
    sign_ = Sign::MINUS;
    num *= -1;
  }
  if (num == 0) {
    digits_.push_back(0);
  }
  while (num) {
    digits_.push_back(num % kBase);
    num /= kBase;
  }
  norm();
}

BigInteger::BigInteger(const BigInteger& bi) : digits_(bi.digits_), sign_(bi.sign_) {}

BigInteger operator""_bi(unsigned long long num) {
  BigInteger result;
  result.digits_.pop_back();
  if (num == 0) {
    result.digits_.push_back(0);
  }
  while (num) {
    result.digits_.push_back(num % result.kBase);
    num /= result.kBase;
  }
  result.norm();
  return result;
}

BigInteger operator""_bi(const char* num, size_t sz) {
  BigInteger result;
  if (sz == 0) {
    return result;
  }
  std::string temp;
  int end = 0;
  int baseCount = log10(result.kBase);
  if (num[0] == '+') {
    result.sign_ = BigInteger::Sign::PLUS;
    end++;
  } else if (num[0] == '-') {
    result.sign_ = BigInteger::Sign::MINUS;
    end++;
  }
  result.digits_.pop_back();
  for (int i = static_cast<int>(sz); i >= end; --i) {
    if (num[i] == '\0') {
      continue;
    }
    temp += num[i];
    if (static_cast<int>(temp.length()) == baseCount) {
      std::reverse(temp.begin(), temp.end());
      result.digits_.push_back(std::stoi(temp));
      temp = "";
    }
  }
  if (!temp.empty()) {
    std::reverse(temp.begin(), temp.end());
    result.digits_.push_back(std::stoi(temp));
  }
  return result;
}

BigInteger& BigInteger::operator=(const BigInteger& bi) {
  digits_ = bi.digits_;
  sign_ = bi.sign_;
  return *this;
}

BigInteger& BigInteger::operator+=(const BigInteger& bi) {
  if (*this == 0) {
    *this = bi;
    return *this;
  } else if (bi == 0) {
    return *this;
  }
  if (sign_ != bi.sign_) {
    return *this -= (-bi);
  }
  int64_t remainder = 0;
  for (int i = 0; i < bi.Size() || remainder; ++i) {
    if (i == Size()) {
      digits_.push_back(0);
    }
    digits_[i] += remainder + (i < bi.Size() ? bi.digits_[i] : 0);
    remainder = digits_[i] >= kBase;
    if (remainder) {
      digits_[i] -= kBase;
    }
  }
  norm();
  return *this;
}

BigInteger& BigInteger::operator-=(const BigInteger& bi) {
  if (bi == 0) {
    return *this;
  }
  if (sign_ != bi.sign_) {
    return *this += (-bi);
  }
  if (modulo() <= bi.modulo()) {
    sign_ = sign_ == BigInteger::Sign::PLUS ? BigInteger::Sign::MINUS : BigInteger::Sign::PLUS;
    digits_ = subtract(bi, *this).digits_;
  } else {
    digits_ = subtract(*this, bi).digits_;
  }
  norm();
  return *this;
}

BigInteger& BigInteger::operator*=(const BigInteger& bi) {
  BigInteger temp;
  temp.digits_.resize(Size() + bi.Size());
  if (sign_ != bi.sign_) {
    temp.sign_ = BigInteger::Sign::MINUS;
  } else {
    temp.sign_ = BigInteger::Sign::PLUS;
  }
  for (int i = 0; i < Size(); ++i) {
    int64_t remainder = 0;
    for (int j = 0; j < bi.Size() || remainder; ++j) {
      while (i + j >= temp.Size()) {
        temp.digits_.push_back(0);
      }
      int64_t value = temp.digits_[i + j] + digits_[i] * (j < bi.Size() ? bi.digits_[j] : 0) + remainder;
      temp.digits_[i + j] = value % kBase;
      remainder = value / kBase;
    }
  }
  while (temp.Size() > 1 && temp.digits_.back() == 0) {
    temp.digits_.pop_back();
  }
  swap(temp, *this);
  norm();
  return *this;
}

BigInteger BigInteger::multByInt(int64_t num) {
  BigInteger res = *this;
  long long remainder = 0;
  for (int i = 0; i < res.Size() || remainder; ++i) {
    if (i == res.Size()) {
      res.digits_.push_back(0);
    }
    remainder += res.digits_[i] * num;
    res.digits_[i] = remainder % kBase;
    remainder /= kBase;
  }
  while (res.Size() > 1 && res.digits_.back() == 0) {
    res.digits_.pop_back();
  }
  res.norm();
  return res;
}

BigInteger& BigInteger::operator/=(const BigInteger& bi) {
  if (sign_ != bi.sign_) {
    sign_ = BigInteger::Sign::MINUS;
  } else {
    sign_ = BigInteger::Sign::PLUS;
  }
  BigInteger bimod = bi.modulo();
  if (digits_ == bi.digits_) {
    digits_ = {1};
    norm();
    return *this;
  }
  if (moduloCmp(*this, bi)) {
    digits_ = {0};
    sign_ = BigInteger::Sign::PLUS;
    norm();
    return *this;
  }
  BigInteger temp;
  BigInteger result;
  result.digits_.pop_back();
  for (int i = Size() - 1; i >= 0; --i) {
    temp.digits_.insert(temp.digits_.begin(), digits_[i]);
    while (temp.Size() > 1 && temp.digits_.back() == 0) {
      temp.digits_.pop_back();
    }
    long long l = 0;
    long long r = kBase;
    if (temp >= bimod) {
      while (r - l > 1) {
        long long mid = (r + l) / 2;
        BigInteger denominator = bimod * mid;
        if (temp - denominator >= 0) {
          l = mid;
        } else {
          r = mid;
        }
      }
    }
    result.digits_.push_back(l);
    if (l > 0) {
      temp -= bimod * l;
    }
  }
  std::reverse(result.digits_.begin(), result.digits_.end());
  digits_ = result.digits_;
  while (Size() > 1 && digits_.back() == 0) {
    digits_.pop_back();
  }
  norm();
  return *this;
}

BigInteger& BigInteger::operator%=(const BigInteger& bi) {
  BigInteger temp = *this / bi;
  temp *= bi;
  *this -= temp;
  norm();
  return *this;
}

BigInteger operator+(const BigInteger& bi1, const BigInteger& bi2) {
  BigInteger result;
  if (bi1.Size() <= bi2.Size()) {
    result = bi2;
    result += bi1;
  } else {
    result = bi1;
    result += bi2;
  }
  return result;
}

BigInteger operator-(const BigInteger& bi1, const BigInteger& bi2) {
  BigInteger result = bi1;
  result -= bi2;
  return result;
}

BigInteger operator*(const BigInteger& bi1, const BigInteger& bi2) {
  BigInteger result;
  if (bi1.Size() <= bi2.Size()) {
    result = bi2;
    result *= bi1;
  } else {
    result = bi1;
    result *= bi2;
  }
  return result;
}

BigInteger operator/(const BigInteger& bi1, const BigInteger& bi2) {
  BigInteger result = bi1;
  result /= bi2;
  return result;
}

BigInteger operator%(const BigInteger& bi1, const BigInteger& bi2) {
  BigInteger result = bi1;
  result %= bi2;
  return result;
}

BigInteger& BigInteger::operator++() {
  *this += 1;
  return *this;
}

BigInteger BigInteger::operator++(int) {
  BigInteger copy = *this;
  ++*this;
  return copy;
}

BigInteger& BigInteger::operator--() {
  *this -= 1;
  return *this;
}

BigInteger BigInteger::operator--(int) {
  BigInteger copy = *this;
  --*this;
  return copy;
}

BigInteger BigInteger::operator-() const {
  BigInteger copy = *this;
  if (copy.sign_ == BigInteger::Sign::PLUS) {
    copy.sign_ = BigInteger::Sign::MINUS;
  } else {
    copy.sign_ = BigInteger::Sign::PLUS;
  }
  copy.norm();
  return copy;
}

bool operator==(const BigInteger& bi1, const BigInteger& bi2) {
  return !(bi1 != bi2);
}

bool operator!=(const BigInteger& bi1, const BigInteger& bi2) {
  return bi1 < bi2 || bi1 > bi2;
}

bool operator<(const BigInteger& bi1, const BigInteger& bi2) {
  if (bi1.sign_ != bi2.sign_) {
    return bi1.sign_ == BigInteger::Sign::MINUS;
  }
  bool result = bi1.sign_ == BigInteger::Sign::MINUS;
  if (bi1.Size() != bi2.Size()) {
    return result ^ (bi1.Size() < bi2.Size());
  }
  for (int i = bi1.Size() - 1; i >= 0; --i) {
    if (bi1.digits_[i] == bi2.digits_[i]) {
      continue;
    }
    return result ^ (bi1.digits_[i] < bi2.digits_[i]);
  }
  return false;
}

bool operator>(const BigInteger& bi1, const BigInteger& bi2) {
  return bi2 < bi1;
}

bool operator<=(const BigInteger& bi1, const BigInteger& bi2) {
  return !(bi1 > bi2);
}

bool operator>=(const BigInteger& bi1, const BigInteger& bi2) {
  return !(bi1 < bi2);
}

std::string BigInteger::toString() const {
  std::string str;
  if (sign_ == BigInteger::Sign::MINUS) {
    str += '-';
  }
  for (int i = static_cast<int>(digits_.size()) - 1; i >= 0; --i) {
    std::string temp = std::to_string(digits_[i]);
    if (i != static_cast<int>(digits_.size()) - 1) {
      while (temp.size() < log10(kBase)) {
        temp = "0" + temp;
      }
    }
    str += temp;
  }
  return str;
}

int BigInteger::Size() const {
  return static_cast<int>(digits_.size());
}

bool BigInteger::moduloCmp(const BigInteger& bi1, const BigInteger& bi2) {
  if (bi1.Size() != bi2.Size()) {
    return bi1.Size() < bi2.Size();
  }
  for (int i = bi1.Size() - 1; i >= 0; --i) {
    if (bi1.digits_[i] == bi2.digits_[i]) {
      continue;
    }
    return bi1.digits_[i] < bi2.digits_[i];
  }
  return false;
}

std::ostream& operator<<(std::ostream& stream, const BigInteger& bi) {
  stream << bi.toString();
  return stream;
}

std::istream& operator>>(std::istream& stream, BigInteger& bi) {
  std::string temp;
  bi.sign_ = BigInteger::Sign::PLUS;
  stream >> temp;
  if (temp[0] == '+') {
    bi.sign_ = BigInteger::Sign::PLUS;
    temp = temp.substr(1);
  } else if (temp[0] == '-') {
    bi.sign_ = BigInteger::Sign::MINUS;
    temp = temp.substr(1);
  }
  std::reverse(temp.begin(), temp.end());
  bi.digits_.clear();
  int baseCount = log10(bi.kBase);
  for (int i = 0; i < static_cast<int>(temp.length()); i += baseCount) {
    std::string add = temp.substr(i, std::min(baseCount, static_cast<int>(temp.length()) - i));
    std::reverse(add.begin(), add.end());
    bi.digits_.push_back(stoi(add));
  }
  return stream;
}

BigInteger BigInteger::modulo() const {
  BigInteger copy = *this;
  if (sign_ == Sign::MINUS) {
    copy.sign_ = Sign::PLUS;
  }
  return copy;
}

void BigInteger::swap(BigInteger& bi1, BigInteger& bi2) {
  std::swap(bi1.digits_, bi2.digits_);
  std::swap(bi1.sign_, bi2.sign_);
}

void BigInteger::norm() {
  if (Size() == 1 && digits_[0] == 0) {
    sign_ = Sign::PLUS;
  }
}

BigInteger BigInteger::divByInt(int64_t num) {
  BigInteger res;
  res.sign_ = sign_;
  res.digits_.pop_back();
  if (num < 0) {
    res.sign_ = (res.sign_ == Sign::PLUS ? Sign::MINUS : Sign:: PLUS);
  }
  long long cur = 0;
  for (int i = Size() - 1; i >= 0; --i) {
    cur = digits_[i] + cur * kBase;
    res.digits_.insert(res.digits_.begin(), cur / num);
    cur %= num;
  }
  while (res.digits_.size() > 1 && res.digits_.back() == 0) {
    res.digits_.pop_back();
  }
  res.norm();
  return res;
}

class Rational {
 public:
  Rational();
  Rational(BigInteger);
  Rational(int);
  Rational(const Rational&);
  Rational& operator=(const Rational&);
  Rational& operator+=(const Rational&);
  Rational& operator-=(const Rational&);
  Rational& operator*=(const Rational&);
  Rational& operator/=(const Rational&);
  Rational operator-() const;
  std::string toString();
  std::string asDecimal(int);
  explicit operator double() {
    double temp = std::stod(asDecimal(300));
    return temp;
  }
  friend bool operator<(const Rational& rat1, const Rational& rat2);
 private:
  BigInteger p;
  BigInteger q;
  void norm();
  BigInteger findDenominator(const BigInteger&, const BigInteger&);
  BigInteger nod(const BigInteger&, const BigInteger&);
};

bool operator==(const Rational& rat1, const Rational& rat2);

bool operator!=(const Rational& rat1, const Rational& rat2);

bool operator<(const Rational& rat1, const Rational& rat2);

bool operator>(const Rational& rat1, const Rational& rat2);

bool operator<=(const Rational& rat1, const Rational& rat2);

bool operator>=(const Rational& rat1, const Rational& rat2);

Rational operator+(const Rational& rat1, const Rational& rat2);

Rational operator-(const Rational& rat1, const Rational& rat2);

Rational operator*(const Rational& rat1, const Rational& rat2);

Rational operator/(const Rational& rat1, const Rational& rat2);

Rational::Rational() : p(0), q(1) {}

Rational::Rational(BigInteger bi) : p(bi), q(1) {}

Rational::Rational(int x) : p(x), q(1) {}

Rational::Rational(const Rational& rat) : p(rat.p), q(rat.q) {}

Rational& Rational::operator=(const Rational& rat) {
  p = rat.p;
  q = rat.q;
  norm();
  return *this;
}

Rational& Rational::operator+=(const Rational& rat) {
  BigInteger den = findDenominator(q, rat.q);
  p = p * (den / q) + rat.p * (den / rat.q);
  q = den;
  norm();
  return *this;
}

Rational& Rational::operator-=(const Rational& rat) {
  BigInteger den = findDenominator(q, rat.q);
  p = p * (den / q) - rat.p * (den / rat.q);
  q = den;
  norm();
  return *this;
}

Rational& Rational::operator*=(const Rational& rat) {
  p *= rat.p;
  q *= rat.q;
  norm();
  return *this;
}

Rational& Rational::operator/=(const Rational& rat) {
  p *= rat.q;
  q *= rat.p;
  norm();
  return *this;
}

Rational operator+(const Rational& rat1, const Rational& rat2) {
  Rational res = rat1;
  res += rat2;
  return res;
}

Rational operator-(const Rational& rat1, const Rational& rat2) {
  Rational res = rat1;
  res -= rat2;
  return res;
}

Rational operator*(const Rational& rat1, const Rational& rat2) {
  Rational res = rat1;
  res *= rat2;
  return res;
}

Rational operator/(const Rational& rat1, const Rational& rat2) {
  Rational res = rat1;
  res /= rat2;
  return res;
}

bool operator==(const Rational& rat1, const Rational& rat2) {
  return !(rat1 != rat2);
}

bool operator!=(const Rational& rat1, const Rational& rat2) {
  return rat1 < rat2 || rat1 > rat2;
}

bool operator<(const Rational& rat1, const Rational& rat2) {
  return rat1.p * rat2.q < rat2.p * rat1.q;
}

bool operator>(const Rational& rat1, const Rational& rat2) {
  return rat2 < rat1;
}

bool operator<=(const Rational& rat1, const Rational& rat2) {
  return !(rat1 > rat2);
}

bool operator>=(const Rational& rat1, const Rational& rat2) {
  return !(rat1 < rat2);
}

std::string Rational::toString() {
  std::string res;
  BigInteger d = nod(p.modulo(), q.modulo());
  p /= d;
  q /= d;
  res += p.toString();
  if (q != 1) {
    res += "/" + q.toString();
  }
  return res;
}

std::string Rational::asDecimal(int precision = 0) {
  std::string res;
  BigInteger d = nod(p.modulo(), q.modulo());
  p /= d;
  q /= d;
  Rational copy = *this;
  BigInteger step(1);
  for (int i = 0; i < precision; ++i) {
    step *= 10;
  }
  copy *= step;
  res = (copy.p / copy.q).toString();
  copy /= step;
  bool minus = false;
  if (res[0] == '-') {
    minus = true;
    res = res.substr(1);
  }
  std::reverse(res.begin(), res.end());
  while (static_cast<int>(res.size()) < precision + 1) {
    res.push_back('0');
  }
  if (precision > 0) {
    res = res.substr(0, precision) + "." + res.substr(precision);
  }
  std::reverse(res.begin(), res.end());
  if (minus) {
    res = "-" + res;
  }
  return res;
}

void Rational::norm() {
  if (q < 0) {
    q *= -1;
    p *= -1;
  }
}

Rational Rational::operator-() const {
  Rational copy = *this;
  copy *= -1;
  return copy;
}

BigInteger Rational::nod(const BigInteger& x, const BigInteger& y) {
  return y == 0 ? x : nod(y, x % y);
}

BigInteger Rational::findDenominator(const BigInteger& a, const BigInteger& b) {
  BigInteger d = nod(a, b);
  return a * b / d;
}

#endif /* biginteger_h */