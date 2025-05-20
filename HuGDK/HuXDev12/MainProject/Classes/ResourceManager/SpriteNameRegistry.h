//
// SpriteNameRegistry.h
//‰æ‘œŒÄ‚Ño‚µ—penum

#pragma once

enum  SpriteName {
#define ITEM(name,adress)name,
#include "defineFile/SpriteInfo.def"
#undef ITEM
};

enum  FontName {
#define ITEM(name,adress)name,
#include "defineFile/FontInfo.def"
#undef ITEM
};