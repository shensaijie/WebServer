#pragma once

class noncopyable {
protected:
    noncopyable() {}
private:
    noncopyable(const noncopyable&) = delete;
    void operator=(const noncopyable&) = delete;
};

class copyable {
};
