//
// SpriteNameRegistry.h
//�摜�Ăяo���penum

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