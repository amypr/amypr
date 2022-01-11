#pragma once

#include <array>
#include <utility>

#include "common.hpp"
#include "components.hpp"
#include "tiny_ecs.hpp"
#include <render_system.hpp>

//#include <ft2build.h>
//#include FT_FREETYPE_H

// System responsible for rendering all the
// text in the game
class Text {
	/**
	 * Render text on the screen
	 */

	// starts the game
	int init();

	// The contents of the Text object, as a ASCII- or UTF-8-encoded string
    std::string content;

    // A shared_ptr to the Text's font.
    // The font may be changed at any time, but must not be null.
    // std::shared_ptr<TextFont> font;

    // The on-screen position of the left edge of the first glyph's baseline,
    // relative to the bottom left corner.
    glm::vec2 position;

    // The text's scale. Default value is 1.0f
    float scale;

    // The text's colour. Default value of {0.0f, 0.0f, 0.0f} (black)
    glm::vec3 colour;

public:
	// Draw text
	// void RenderText();

    //void StoreCharData(FT_Face face);

private:
	Entity number_chars_state_entity;
	// Entity scoreboard_state_entity;
};
