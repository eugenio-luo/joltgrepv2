module;

#include <cstddef>

#include "debug.h"
#include "test.h"

export module buf;

template <size_t BUF_SIZE>
    requires (BUF_SIZE % 2 == 0 && BUF_SIZE > 0)
class Buf {
private:
    char bytes[BUF_SIZE];

public:
    char& operator[](int i) {
        assertSpan(i, BUF_SIZE);
        return bytes[i];
    }
};

// TODO: grow multiple pages?
export
template <int BUF_SIZE, int N>
    requires (N > 0)
class MultiBuf {
private:
    Buf<BUF_SIZE> bufs[N];

    // points to first free byte
    int allocStart{0};
    // points to next free byte
    int allocEnd{0};

    int offset{0};

    int getMaxSize(void) const {
        return N * BUF_SIZE;
    }

    void assertInvariants(void) const {
        assertAtLeast(allocStart, 0);
        assertBound(allocStart, offset, allocEnd);
        assertSpan(allocEnd - allocStart - 1, getMaxSize());
    }

    int getOutIdx(int idx) const {
        int outIdx = (idx / BUF_SIZE) % N;
        assertSpan(outIdx, N);
        return outIdx;
    }

    int getInnIdx(int idx) const {
        int innIdx = idx % BUF_SIZE;
        assertSpan(innIdx, BUF_SIZE);
        return innIdx;
    }

public:
    size_t getPageSize(void) const {
        return BUF_SIZE;
    }

    size_t getPageN(void) const {
        return N;
    }

    int change(int n) {
        offset += n;

        assertInvariants();
        return offset;
    }

    int current(void) const {
        return offset;
    }

    bool growNext(void) {
        if (allocEnd + BUF_SIZE - allocStart > getMaxSize()) {
            if (offset - allocStart < BUF_SIZE) {
                return false;
            }

            allocStart += BUF_SIZE;
        }

        allocEnd += BUF_SIZE;
        assertInvariants();
        return true;
    }

    bool growBack(void) {
        if (allocStart < BUF_SIZE) {
            return false;
        }

        if (allocEnd - (allocStart - BUF_SIZE) > getMaxSize()) {
            if (allocEnd - offset < BUF_SIZE) {
                return false;
            }

            allocEnd -= BUF_SIZE;
        }

        allocStart -= BUF_SIZE;
        assertInvariants();
        return true;
    }

    char& operator[](int i) {
        int idx = offset + i;
        assertBound(allocStart, idx, allocEnd);

        int outIdx = getOutIdx(idx);
        int innIdx = getInnIdx(idx);
        return bufs[outIdx][innIdx];
    }
};

TEST_FN(MultiBufBasic) {
    MultiBuf<2, 2> bufs;
    bufs.growNext();
    bufs[0] = 1;
    bufs.growNext();
    assert(bufs[0] == 1, "bufs[0] == 1 instead: {0}", bufs[0]);
}

TEST_FN(MultiBufNoGrow) {
    MultiBuf<2, 2> bufs;
    try {
        bufs[0] = 1;
        assert(false, "should have throw exception");
    } catch (const TestError& err) {
        return;
    }
}

TEST_FN(MultiBufOvergrow) {
    MultiBuf<2, 2> bufs;
    bufs.growNext();
    bufs.growNext();
    bool success = bufs.growNext();
    assert(!success, "bufs grow should fail");
}

TEST_FN(MultiBufTwoSideOvergrow1) {
    MultiBuf<2, 2> bufs;
    bufs.growNext();
    bufs.growBack();
    bool success = bufs.growNext();
    assert(success, "bufs grow should succeed");
}

TEST_FN(MultiBufTwoSideOvergrow2) {
    MultiBuf<2, 2> bufs;
    bufs.growBack();
    bufs.growNext();
    bool success = bufs.growBack();
    assert(!success, "bufs grow should fail");
}

TEST_FN(MultiBufOvergrowWithOffset) {
    MultiBuf<2, 2> bufs;
    bufs.growNext();
    bufs.growNext();
    bufs.change(bufs.getPageSize());
    bool success = bufs.growNext();
    assert(success, "bufs grow should succeed");
}
