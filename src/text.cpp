// internal
//#include "text.hpp"
//#include "render_system.hpp"
//
//#include <SDL.h>
//#include "tiny_ecs_registry.hpp"
//#include "components.hpp"

//int Text::init() {
//	registry.numberCharacters.emplace(number_chars_state_entity);
	// FT_Library ft;
    // if (FT_Init_FreeType(&ft))
    // {
    //     // ERROR::FREETYPE: Could not init FreeType Library
    //     return -1;
    // }

    // FT_Face face;
    // if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face))
    // {
    //     // ERROR::FREETYPE: Failed to load font  
    //     return -1;
    // }
    // StoreCharData(face);
    // FT_Done_Face(face);
    // FT_Done_FreeType(ft);
//}

//void Text::StoreCharData(FT_Face face) {
//    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
  
    // for (unsigned char c = 0; c < 128; c++)
    // {
    //     // load character glyph 
    //     if (FT_Load_Char(face, c, FT_LOAD_RENDER))
    //     {
    //         // "ERROR::FREETYTPE: Failed to load Glyph"
    //         continue;
    //     }
    //     // generate texture
    //     unsigned int texture;
    //     glGenTextures(1, &texture);
    //     glBindTexture(GL_TEXTURE_2D, texture);
    //     glTexImage2D(
    //         GL_TEXTURE_2D,
    //         0,
    //         GL_RED,
    //         face->glyph->bitmap.width,
    //         face->glyph->bitmap.rows,
    //         0,
    //         GL_RED,
    //         GL_UNSIGNED_BYTE,
    //         face->glyph->bitmap.buffer
    //     );
    //     // set texture options
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //     // now store character for later use
    //     NumberCharacter character = {
    //         texture, 
    //         glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
    //         glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
    //         face->glyph->advance.x
    //     };
	//     NumberCharacters &numberCharacters = registry.numberCharacters.components[0];
    //     numberCharacters.numberCharacters.insert(std::pair<char, NumberCharacter>(c, character));
    // }
//}

// void Text::RenderText(std::string text, float x, float y, float scale, glm::vec3 color)
// {
	// const GLuint program = (GLuint)effects[(GLuint)EFFECT_ASSET_ID::TEXT]
    // // activate corresponding render state
    // glUniform3f(glGetUniformLocation(program, "textColor"), color.x, color.y, color.z);
    // glActiveTexture(GL_TEXTURE0);
    // glBindVertexArray(VAO);

    // // iterate through all characters
    // std::string::const_iterator c;
    // for (c = text.begin(); c != text.end(); c++)
    // {
    //     Character ch = Characters[*c];

    //     float xpos = x + ch.Bearing.x * scale;
    //     float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

    //     float w = ch.Size.x * scale;
    //     float h = ch.Size.y * scale;
    //     // update VBO for each character
    //     float vertices[6][4] = {
    //         { xpos,     ypos + h,   0.0f, 0.0f },            
    //         { xpos,     ypos,       0.0f, 1.0f },
    //         { xpos + w, ypos,       1.0f, 1.0f },

    //         { xpos,     ypos + h,   0.0f, 0.0f },
    //         { xpos + w, ypos,       1.0f, 1.0f },
    //         { xpos + w, ypos + h,   1.0f, 0.0f }           
    //     };
    //     // render glyph texture over quad
    //     glBindTexture(GL_TEXTURE_2D, ch.textureID);
    //     // update content of VBO memory
    //     glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //     glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
    //     glBindBuffer(GL_ARRAY_BUFFER, 0);
    //     // render quad
    //     glDrawArrays(GL_TRIANGLES, 0, 6);
    //     // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
    //     x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    // }
    // glBindVertexArray(0);
    // glBindTexture(GL_TEXTURE_2D, 0);
// }