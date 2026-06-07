#include "string_algorithm.hpp"

namespace my::str {

static void max_suffix(const u8* x, const usize m, const bool order, usize& pos, usize& period) {
    std::ptrdiff_t ms = -1;
    usize j = 0;
    usize k = 1;
    usize p = 1;

    while (j + k < m) {
        const u8 a = x[j + k];
        const u8 b = x[static_cast<usize>(ms + static_cast<std::ptrdiff_t>(k))];

        if (a == b) {
            if (k == p) {
                j += p;
                k = 1;
            } else {
                ++k;
            }
        } else if ((a > b) == order) {
            j += k;
            k = 1;
            p = static_cast<usize>(static_cast<std::ptrdiff_t>(j) - ms);
        } else {
            ms = static_cast<std::ptrdiff_t>(j);
            j = static_cast<usize>(ms + 1);
            k = 1;
            p = 1;
        }
    }

    pos = (ms < 0) ? 0 : static_cast<usize>(ms);
    period = p;
}

Option<usize> twoway_find(const u8* hay, const usize hlen, const u8* pat, const usize plen) {
    if (plen == 0) return Option<usize>::Some(0);
    if (plen > hlen) return Option<usize>::None();

    usize ms1 = 0, p1 = 0;
    usize ms2 = 0, p2 = 0;
    max_suffix(pat, plen, true, ms1, p1);
    max_suffix(pat, plen, false, ms2, p2);

    usize crit;
    usize period;
    if (ms1 > ms2) {
        crit = ms1;
        period = p1;
    } else {
        crit = ms2;
        period = p2;
    }

    usize pos = 0;
    bool is_periodic = false;
    if (period > 0 && period + crit < plen) {
        is_periodic = (std::memcmp(pat, pat + period, crit + 1) == 0);
    }
    if (is_periodic) {
        usize memory = 0;
        while (pos + plen <= hlen) {
            usize i = std::max(crit, memory);
            while (i < plen && pat[i] == hay[pos + i]) {
                ++i;
            }
            if (i >= plen) {
                i = crit;
                while (i > memory && pat[i - 1] == hay[pos + i - 1]) {
                    --i;
                }
                if (i <= memory) {
                    return Option<usize>::Some(pos);
                }
                pos += period;
                memory = plen - period;
            } else {
                pos += (i > crit) ? (i - crit) : 1;
                memory = 0;
            }
        }
    } else {
        while (pos + plen <= hlen) {
            usize i = crit;
            while (i < plen && pat[i] == hay[pos + i]) {
                ++i;
            }
            if (i >= plen) {
                i = crit;
                while (i > 0 && pat[i - 1] == hay[pos + i - 1]) {
                    --i;
                }
                if (i == 0) {
                    return Option<usize>::Some(pos);
                }
                pos += i;
            } else {
                pos += i - crit + 1;
            }
        }
    }

    return Option<usize>::None();
}

static util::Vec<usize> kmp_next_array(const u8* pat, const usize plen) {
    util::Vec<usize> next(plen, 0);
    for (usize i = 1, j = 0; i < plen; ++i) {
        // 失配，j按照next数组回跳
        while (j > 0 && pat[i] != pat[j]) {
            j = next[j - 1];
        }
        j += pat[i] == pat[j]; // 匹配，j前进
        next[i] = j;
    }
    return next;
}

Option<usize> kmp_find(const u8* hay, const usize hlen, const u8* pat, const usize plen) {
    if (pat == nullptr || plen == 0) return Option<usize>::None();
    const auto next = kmp_next_array(pat, plen);

    for (usize i = 0, j = 0; i < hlen; ++i) {
        // 失配，j按照next数组回跳
        while (j > 0 && hay[i] != pat[j]) {
            j = next[j - 1];
        }
        j += hay[i] == pat[j]; // 匹配，j前进
        // 模式串匹配完，返回文本串匹配起点
        if (j == plen) {
            return Option<usize>::Some(i - plen + 1);
        }
    }
    return Option<usize>::None();
}

util::Vec<usize> kmp_find_all(const u8* hay, const usize hlen, const u8* pat, const usize plen) {
    util::Vec<usize> res;
    if (pat == nullptr || plen == 0) return res;
    const auto next = kmp_next_array(pat, plen);

    for (usize i = 0, j = 0; i < hlen; ++i) {
        // 失配，j按照next数组回跳
        while (j > 0 && hay[i] != pat[j]) {
            j = next[j - 1];
        }
        j += hay[i] == pat[j]; // 匹配，j前进
        // 模式串匹配完，返回文本串匹配起点
        if (j == plen) {
            res.push(i - plen + 1);
            j = next[j - 1];
        }
    }
    return res;
}

} // namespace my::str
