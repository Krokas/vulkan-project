#include "font.h"

#include <corecrt_malloc.h>

#include <cstdio>
#include <stdexcept>

#include "file.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <ft2build.h>
#include <stb_image_write.h>

#include <fstream>
#include FT_FREETYPE_H

Font::Font(Device* device, const std::string& fontName, const unsigned int fontSize)
    : fontSize{fontSize}, texture{device} {
  bitmapInfo.bitmapSize.width = 512;
  bitmapInfo.bitmapSize.height = 256;
  bitmapInfo.firstCharacter = 32;
  bitmapInfo.characterCount = 96;

  /* prepare font */

  FT_Library fontLibrary;
  FT_Init_FreeType(&fontLibrary);

  FT_Face fontFace;
  FT_New_Face(fontLibrary, File::getFontFilePath(fontName).c_str(), 0, &fontFace);
  FT_Set_Pixel_Sizes(fontFace, 0, fontSize);

  int padding = 2;
  int row = 0;
  int col = padding;

  const int textureWidth = 512;
  char textureBuffer[textureWidth * textureWidth];
  for (FT_ULong glyphIdx = 32; glyphIdx < 127; ++glyphIdx) {
    FT_UInt glyphIndex = FT_Get_Char_Index(fontFace, glyphIdx);
    FT_Load_Glyph(fontFace, glyphIndex, FT_LOAD_DEFAULT);
    FT_Error error = FT_Render_Glyph(fontFace->glyph, FT_RENDER_MODE_NORMAL);

    if (col + fontFace->glyph->bitmap.width + padding >= 512) {
      col = padding;
      row += fontSize;
    }

    // Font Height
    fontHeight =
        max((fontFace->size->metrics.ascender - fontFace->size->metrics.descender) >> 6,
            fontHeight);

    for (unsigned int y = 0; y < fontFace->glyph->bitmap.rows; ++y) {
      for (unsigned int x = 0; x < fontFace->glyph->bitmap.width; ++x) {
        textureBuffer[(row + y) * textureWidth + col + x] =
            fontFace->glyph->bitmap.buffer[y * fontFace->glyph->bitmap.width + x];
      }
    }

    Glyph* glyph = &glyphs[glyphIdx];
    glyph->textureCoords = {col, row};
    glyph->size = {(int)fontFace->glyph->bitmap.width, (int)fontFace->glyph->bitmap.rows};
    glyph->advance = {
        (float)(fontFace->glyph->advance.x >> 6),
        (float)(fontFace->glyph->advance.y >> 6)};
    glyph->offset = {
        (float)fontFace->glyph->bitmap_left,
        (float)fontFace->glyph->bitmap_top,
    };

    col += fontFace->glyph->bitmap.width + padding;
  }

  FT_Done_Face(fontFace);
  FT_Done_FreeType(fontLibrary);

  texture.createFromData((unsigned char*)textureBuffer, textureWidth, textureWidth, 4);
  // texture.addTexturePath("texture.jpg");

  stbi_write_jpg("out.jpg", textureWidth, textureWidth, 4, textureBuffer, textureWidth * 3);
}

void Font::getGlyphCodepoint() {
  // auto glyphBitmap = stbtt_GetCodepointSDF()
}

std::vector<Model::Vertex> Font::getVertices(
    const std::string& text, const glm::vec2& position, VkExtent2D extent) {
  std::vector<Model::Vertex> vertices{};
  /*glm::vec2 startingPosition = { 0.0f, 0.0f };

  glm::vec2 screenDimention{};

  screenDimention.x = 2.0f / extent.width;
  screenDimention.y = 2.0f / extent.height;*/

  /*glm::vec2 advance = position;

  for (int i = 0; i < text.size(); i++) {
      Model::Vertex v0, v1, v2, v3;

      v0.position = { advance.x, advance.y + screenDimention.y, 0.0f };
      v1.position = { advance.x, advance.y, 0.0f };
      v2.position = { advance.x + screenDimention.x, advance.y + screenDimention.y, 0.0f };
      v3.position = { advance.x + screenDimention.x, advance.y, 0.0f };

      vertices.push_back(v0);
      vertices.push_back(v1);
      vertices.push_back(v2);
      vertices.push_back(v3);

      advance.x += screenDimention.x + 0.01f;
  }*/

  /*int ascent, baseline, ch = 0;
  float xpos = position.x;
  float scale = screenDimention.y;

  scale = stbtt_ScaleForPixelHeight(&info, (float)fontSize);
  stbtt_GetFontVMetrics(&info, &ascent, 0, 0);
  baseline = (int)(ascent * scale);*/

  // while (text[ch])
  //{
  //     int advance, lsb, x0, y0, x1, y1, codepoint;
  //     codepoint = text[ch];
  //     float x_shift = position.x - (float)floor(xpos);
  //     stbtt_GetCodepointHMetrics(&info, codepoint, &advance, &lsb);
  //     stbtt_GetCodepointBitmapBoxSubpixel(&info, codepoint, scale, scale, x_shift, 0, &x0, &y0,
  //     &x1, &y1);

  //    auto charInfo = characterData[codepoint];

  //    Model::Vertex v0, v1, v2, v3;
  //    //stbtt_aligned_quad quad;
  //    //stbtt_GetBakedQuad(characterData, 512, 512, textString[i] - 32, &startingPosition.x,
  //    &startingPosition.y, &quad, 1);//1=opengl & d3d10+,0=d3d9

  //    // top-left         x0y0
  //    // bottom-left      x0y1
  //    // bottom-right     x1y1
  //    // top-right        x1y0

  //    v0.position = { (x0 * screenDimention.x + xpos), (y0 * screenDimention.y) + position.y, 0.0f
  //    }; v1.position = { (x0 * screenDimention.x + xpos), (y1 * screenDimention.y) + position.y,
  //    0.0f }; v2.position = { (x1 * screenDimention.x + xpos), (y1 * screenDimention.y) +
  //    position.y, 0.0f }; v3.position = { (x1 * screenDimention.x + xpos), (y0 *
  //    screenDimention.y) + position.y, 0.0f };

  //    // x0, y0, x1, y1

  //    // 1 - bitmapInfo.bitmapSize.width
  //    // x - x0y0

  //    // x = x0 * 1 / bitmapInfo.bitmapSize.width

  //    glm::vec2 sPos = { 0.0f, 0.0f };
  //    stbtt_aligned_quad quad;
  //    auto characterToGetQuad = text[ch] - 32;
  //    stbtt_GetBakedQuad(characterData, 512, 256, characterToGetQuad, &sPos.x, &sPos.y, &quad, 1);

  //    v0.uv = { quad.s0, quad.t0 };
  //    v1.uv = { quad.s1, quad.t0 };
  //    v2.uv = { quad.s1, quad.t1 };
  //    v3.uv = { quad.s0, quad.t1 };

  //    /*v0.uv = { charInfo.x0 / bitmapInfo.bitmapSize.width, charInfo.y0 /
  //    bitmapInfo.bitmapSize.height }; v1.uv = { charInfo.x0 / bitmapInfo.bitmapSize.width,
  //    charInfo.y1 / bitmapInfo.bitmapSize.height }; v2.uv = { charInfo.x1 /
  //    bitmapInfo.bitmapSize.width, charInfo.y1 / bitmapInfo.bitmapSize.height }; v3.uv = {
  //    charInfo.x1 / bitmapInfo.bitmapSize.width, charInfo.y0 / bitmapInfo.bitmapSize.height };*/

  //  /*  v0.uv = { 0, 0 };
  //    v1.uv = { 0, 1 };
  //    v2.uv = { 1, 1 };
  //    v3.uv = { 1, 0 };*/

  //

  //    vertices.push_back(v0);
  //    vertices.push_back(v1);
  //    vertices.push_back(v2);
  //    vertices.push_back(v3);

  //    xpos += (advance * scale * screenDimention.x);
  //    if (text[ch + 1])
  //        xpos += scale * stbtt_GetCodepointKernAdvance(&info, text[ch], text[ch + 1]);
  //    ++ch;
  //}

  return vertices;
}
