#pragma once

#include <string>
#include <cstddef>
#include <ostream>
#include <algorithm>

// ---------------------------------------------------------------------
// Helper: how many bytes does a UTF-8 sequence starting with this byte
// occupy? (1-4). Returns 1 for invalid lead bytes so we never get stuck.
// ---------------------------------------------------------------------
inline size_t utf8SequenceLength(unsigned char c) {
    if ((c & 0x80) == 0x00) return 1; // 0xxxxxxx
    if ((c & 0xE0) == 0xC0) return 2; // 110xxxxx
    if ((c & 0xF0) == 0xE0) return 3; // 1110xxxx
    if ((c & 0xF8) == 0xF0) return 4; // 11110xxx
    return 1;                         // invalid lead byte, treat as 1
}

class Utf8String;

// ---------------------------------------------------------------------
// Forward iterator over the *codepoints* of a Utf8String.
// Dereferencing gives you the raw UTF-8 bytes of the current character
// as a std::string. Use codepoint() to get the decoded char32_t value.
// ---------------------------------------------------------------------
class Utf8StringIterator {
public:
    using value_type        = std::string;
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;

    Utf8StringIterator(const std::string& data, size_t bytePos)
        : data_(&data), bytePos_(bytePos) {}

    // The raw bytes that make up the current character (1-4 bytes).
    std::string operator*() const {
        size_t len = currentLength();
        return data_->substr(bytePos_, len);
    }

    // Decoded Unicode codepoint of the current character.
    char32_t codepoint() const {
        unsigned char c = static_cast<unsigned char>((*data_)[bytePos_]);
        size_t len = utf8SequenceLength(c);

        char32_t cp = 0;
        switch (len) {
            case 1: cp = c;        break;
            case 2: cp = c & 0x1F; break;
            case 3: cp = c & 0x0F; break;
            case 4: cp = c & 0x07; break;
        }

        for (size_t i = 1; i < len && bytePos_ + i < data_->size(); ++i) {
            unsigned char cont = static_cast<unsigned char>((*data_)[bytePos_ + i]);
            cp = (cp << 6) | (cont & 0x3F);
        }
        return cp;
    }

    Utf8StringIterator& operator++() {
        bytePos_ += currentLength();
        if (bytePos_ > data_->size()) bytePos_ = data_->size();
        return *this;
    }

    Utf8StringIterator operator++(int) {
        Utf8StringIterator tmp = *this;
        ++(*this);
        return tmp;
    }

    bool operator==(const Utf8StringIterator& other) const {
        return data_ == other.data_ && bytePos_ == other.bytePos_;
    }
    bool operator!=(const Utf8StringIterator& other) const {
        return !(*this == other);
    }

    // Byte offset of the current character within the underlying buffer.
    size_t bytePosition() const { return bytePos_; }

private:
    size_t currentLength() const {
        if (bytePos_ >= data_->size()) return 0;
        size_t len = utf8SequenceLength(static_cast<unsigned char>((*data_)[bytePos_]));
        return std::min(len, data_->size() - bytePos_);
    }

    const std::string* data_;
    size_t bytePos_;
};

// ---------------------------------------------------------------------
// A small UTF-8 aware string wrapper around std::string.
//
// - Stores data as raw UTF-8 bytes.
// - Implicitly convertible from std::string and const char*.
// - operator+ / operator+= for concatenation (with implicit conversions
//   from std::string / const char* on either side).
// - insert() works on *character* (codepoint) indices, not byte indices.
// - length() returns number of codepoints, byteSize() returns raw bytes.
// - begin()/end() give Utf8StringIterator for range-based for loops.
// ---------------------------------------------------------------------
class Utf8String {
private:
    std::string data;

public:
    Utf8String() {}

    // Implicit conversions, as requested.
    Utf8String(const std::string& utf8) : data(utf8) {}
    Utf8String(const char* utf8) : data(utf8 ? utf8 : "") {}

    // Access to the underlying bytes.
    std::string&       asString()       { return data; }
    const std::string& asString() const { return data; }
    const char* c_str() const { return data.c_str(); }

    // Raw byte length vs. number of codepoints ("characters").
    size_t byteSize() const { return data.size(); }
    size_t length() const {
        size_t count = 0;
        for (auto it = begin(); it != end(); ++it) ++count;
        return count;
    }
    bool empty() const { return data.empty(); }

    // Iteration over codepoints.
    Utf8StringIterator begin() const { return Utf8StringIterator(data, 0); }
    Utf8StringIterator end()   const { return Utf8StringIterator(data, data.size()); }

    // Concatenation. The parameter type Utf8String means a std::string
    // or const char* on the right-hand side will convert automatically.
    Utf8String& operator+=(const Utf8String& other) {
        data += other.data;
        return *this;
    }

    Utf8String operator+(const Utf8String& other) const {
        Utf8String result(*this);
        result += other;
        return result;
    }

    // Insert `other` so that it starts at the given *character* index
    // (i.e. charIndex codepoints from the start of the string).
    // If charIndex >= length(), the text is appended at the end.
    Utf8String& insert(size_t charIndex, const Utf8String& other) {
        data.insert(byteIndexForCharIndex(charIndex), other.data);
        return *this;
    }

    // Convert a character (codepoint) index into a byte offset.
    size_t byteIndexForCharIndex(size_t charIndex) const {
        size_t i = 0;
        for (auto it = begin(); it != end(); ++it, ++i) {
            if (i == charIndex) return it.bytePosition();
        }
        return data.size();
    }

    bool operator==(const Utf8String& other) const { return data == other.data; }
    bool operator!=(const Utf8String& other) const { return data != other.data; }
};

// Allow `const char* + Utf8String` and `std::string + Utf8String`.
inline Utf8String operator+(const char* lhs, const Utf8String& rhs) {
    return Utf8String(lhs) + rhs;
}
inline Utf8String operator+(const std::string& lhs, const Utf8String& rhs) {
    return Utf8String(lhs) + rhs;
}

// Stream support, e.g. std::cout << myUtf8String;
inline std::ostream& operator<<(std::ostream& os, const Utf8String& s) {
    return os << s.asString();
}
