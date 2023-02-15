#ifndef string_h
#define string_h

#include <iostream>
#include <cstring>
#include <algorithm>

class String {
 public:
  String (const char*);
  
  String (size_t, const char&);
  
  String ();
    
  String (const String&);
  
  size_t length() const;
  
  void push_back(char);
  
  void pop_back();
  
  char& front();
  
  const char& front() const;
  
  char& back();
  
  const char& back() const;
  
  size_t find(const String&) const;
  
  size_t rfind(const String&) const;
  
  String substr(size_t, size_t) const;
  
  bool empty() const;
  
  void clear();
  
  void shrink_to_fit();
  
  String& operator=(const String&);
  
  char& operator[](size_t);
  
  const char& operator[](size_t) const;
  
  String& operator+=(const String&);
  
  String& operator+=(const char&);
  
  char* data();
  
  const char* data() const;
  
  size_t size() const;
  
  size_t capacity() const;
  
  friend std::ostream& operator<<(std::ostream&, const String&);
  
  ~String();
 private:
  char* arr = nullptr;
  size_t sz = 0;
  size_t cap = 0;
  
  void swap(String&);
  
  void expansion_cap();
};

bool operator==(const String& str1, const String& str2);

bool operator!=(const String& str1, const String& str2);

bool operator<(const String& str1, const String& str2);

bool operator>(const String& str1, const String& str2);

bool operator<=(const String& str1, const String& str2);

bool operator>=(const String& str1, const String& str2);

String::String(const char* str2) {
  for (size_t i = 0;; ++i) {
    if (str2[i] == '\0') {
      break;
    }
    sz++;
    cap++;
  }
  arr = new char[sz + 1];
  memcpy(arr, str2, sz);
  arr[sz] = '\0';
}
  
String::String(size_t n, const char& c) : arr(new char[n + 1]), sz(n), cap(n) {
  memset(arr, c, n);
  arr[n] = '\0';
}

String::String() : arr(new char[1]) {
  arr[0] = '\0';
}

String::String(const String& str2) : String(str2.sz, '\0') {
  memcpy(arr, str2.arr, sz);
}

size_t String::length() const {
  return sz;
}

void String::push_back(char c) {
  expansion_cap();
  arr[sz] = c;
  arr[sz + 1] = '\0';
  sz++;
}

void String::pop_back() {
  arr[sz - 1] = '\0';
  sz--;
}

char& String::front() {
  return arr[0];
}

const char& String::front() const {
  return arr[0];
}

char& String::back() {
  return arr[sz - 1];
}

const char& String::back() const {
  return arr[sz - 1];
}

size_t String::find(const String& substring) const {
  for (size_t i = 0; i <= length() - substring.length(); ++i) {
    bool is_qual = true;
    for (size_t j = i; j < i + substring.length(); ++j) {
      if (arr[j] != substring[j - i]) {
        is_qual = false;
      }
    }
    if (is_qual) {
      return i;
    }
  }
  return length();
}

size_t String::rfind(const String& substring) const {
  for (int i = static_cast<int>(length() - substring.length()); i >= 0; --i) {
    bool is_qual = true;
    for (int j = i; j < i + static_cast<int>(substring.length()); ++j) {
      if (arr[j] != substring[j - i]) {
        is_qual = false;
      }
    }
    if (is_qual) {
      return i;
    }
  }
  return length();
}

String String::substr(size_t start, size_t count) const {
  String res(count, '\0');
  for (size_t i = start; i < start + count; i++) {
    res[i - start] = arr[i];
  }
  return res;
}

bool String::empty() const {
  return length() == 0;
}

void String::clear() {
  sz = 0;
  arr[0] = '\0';
}

void String::shrink_to_fit() {
  String new_str = *this;
  swap(new_str);
}

String& String::operator=(const String& str2) {
  String copy = str2;
  swap(copy);
  return *this;
}

bool operator<(const String& str1, const String& str2) {
  for (size_t i = 0; i < str1.length() && i < str2.length(); ++i) {
    if (str1[i] != str2[i]) {
      return str1[i] < str2[i];
    }
  }
  return str1.length() < str2.length();
}

bool operator>=(const String& str1, const String& str2) {
  return !(str1 < str2);
}

bool operator>(const String& str1, const String& str2) {
  return str2 < str1;
}

bool operator<=(const String& str1, const String& str2) {
  return !(str1 > str2);
}

bool operator==(const String& str1, const String& str2) {
  return !(str1 < str2 || str1 > str2);
}

bool operator!=(const String& str1, const String& str2) {
  return !(str1 == str2);
}

char& String::operator[](size_t index) {
  return arr[index];
}

const char& String::operator[](size_t index) const {
  return arr[index];
}

String& String::operator+=(const String& str2) {
  size_t new_sz = length() + str2.length();
  if (new_sz >= cap) {
    char* temp = new char[new_sz + 1];
    memcpy(temp, arr, sz);
    std::swap(temp, arr);
    delete[] temp;
    sz = new_sz;
    cap = new_sz;
  }
  for (size_t i = new_sz - str2.length(); i <= new_sz; ++i) {
    arr[i] = str2[i - (new_sz - str2.length())];
  }
  return *this;
}

String& String::operator+=(const char& c) {
  push_back(c);
  return *this;
}

String operator+(const String& str1, const char& c) {
  String res = str1;
  res += c;
  return res;
}

String operator+(const char& c, const String& str1) {
  String res;
  res += c;
  res += str1;
  return res;
}

String operator+(const String& str1, const String& str2) {
  String res = str1;
  res += str2;
  return res;
}

char* String::data() {
  return &arr[0];
}

const char* String::data() const {
  return &arr[0];
}

std::ostream& operator<<(std::ostream& stream, const String& string) {
  stream << string.arr;
  return stream;
}

std::istream& operator>>(std::istream& stream, String& string) {
  string  = "";
  char value;
  while (stream.get(value) && !std::isspace(value)) {
    string += value;
  }
  return stream;
}

String::~String() {
  delete[] arr;
}

void String::swap(String &str2) {
  std::swap(arr, str2.arr);
  std::swap(sz, str2.sz);
  std::swap(cap, str2.cap);
}

size_t String::size() const {
  return sz;
}

size_t String::capacity() const {
  return cap;
}

void String::expansion_cap() {
  if (sz < cap) return;
  size_t new_capacity = std::max(cap, static_cast<size_t>(5)) * 2;
  String new_str(new_capacity, '\0');
  memcpy(new_str.arr, arr, sz);
  new_str.sz = sz;
  swap(new_str);
}

#endif