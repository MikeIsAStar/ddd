#include "VI.hh"

#include "common/Arena.hh"
#include "common/DCache.hh"

extern "C" volatile u16 vtr;
extern "C" volatile u16 dcr;
extern "C" volatile u32 vto;
extern "C" volatile u32 vte;
extern "C" volatile u32 tfbl;
extern "C" volatile u32 bfbl;
extern "C" volatile u16 hsw;
extern "C" volatile u16 hsr;
extern "C" volatile u16 visel;

const VI::Color VI::Color::Black = {16, 128, 16, 128};
const VI::Color VI::Color::White = {255, 128, 255, 128};

bool VI::isProgressive() {
    return m_isProgressive;
}

u16 VI::getXFBWidth() {
    return m_xfbWidth;
}

u16 VI::getXFBHeight() {
    return m_xfbHeight;
}

VI::Color VI::readFromXFB(u16 x, u16 y) {
    if (x >= m_xfbWidth || y >= m_xfbHeight) {
        return Color::Black;
    }

    u32 val = m_xfb[y * (m_xfbWidth / 2) + x / 2];
    Color color;
    color.y0 = val >> 24;
    color.u = val >> 16;
    color.y1 = val >> 8;
    color.v = val >> 0;
    return color;
}

void VI::writeToXFB(u16 x, u16 y, Color color) {
    if (x >= m_xfbWidth || y >= m_xfbHeight) {
        return;
    }

    u32 val = color.y0 << 24 | color.u << 16 | color.y1 << 8 | color.v;
    m_xfb[y * (m_xfbWidth / 2) + x / 2] = val;
}

void VI::flushXFB() {
    DCache::Store(m_xfb, m_xfbSize);
}

void VI::Init() {
    s_instance = new (MEM2Arena::Instance(), 0x4) VI;
}

VI *VI::Instance() {
    return s_instance;
}

VI::VI() {
    m_isProgressive = visel & 1 || dcr & 4;
    bool isNtsc = (dcr >> 8 & 3) == 0;
    m_xfbWidth = 640;
    m_xfbHeight = m_isProgressive || isNtsc ? 480 : 574;
    m_xfbSize = m_xfbHeight * (m_xfbWidth + 1) / 2 * sizeof(u32);
    m_xfb = reinterpret_cast<u32 *>(MEM2Arena::Instance()->alloc(m_xfbSize, 0x20));
    for (u16 y = 0; y < m_xfbHeight; y++) {
        for (u16 x = 0; x < m_xfbWidth; x++) {
            writeToXFB(x, y, Color::Black);
        }
    }
    flushXFB();
    vtr = m_xfbHeight << (3 + m_isProgressive) | (vtr & 0xf);
    if (m_isProgressive) {
        vto = 0x6 << 16 | 0x30;
        vte = 0x6 << 16 | 0x30;
    } else if (isNtsc) {
        vto = 0x3 << 16 | 0x18;
        vte = 0x2 << 16 | 0x19;
    } else {
        vto = 0x1 << 16 | 0x23;
        vte = 0x0 << 16 | 0x24;
    }
    hsw = 0x2828;
    hsr = 0x10f5;
    tfbl = 1 << 28 | reinterpret_cast<uintptr_t>(m_xfb) >> 5;
    bfbl = 1 << 28 | reinterpret_cast<uintptr_t>(m_xfb) >> 5;
}

VI *VI::s_instance = nullptr;
