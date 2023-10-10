//
// Created by cervi on 22/08/2022.
//
#include "TitleScreen.hpp"
#include "Engine/Audio.hpp"
#include "Engine/Background.hpp"
#include "Engine/Font.hpp"
#include "Engine/Engine.hpp"
#include "Formats/utils.hpp"

void runTitleScreen() {
    constexpr int fadeInFrames = 30;
    constexpr int holdFrames = 60*5;
    int fadeOutFrames = 30;  // replace on last frame to fadeOutLastFrames
    constexpr int height = 350;
    constexpr int holdLastFrames = 60 * 4;
    constexpr int fadeOutLastFrames = 60 * 4;
    constexpr int scrollFrames = 60 * 7;
    constexpr int introLogoFrames = 300;
    constexpr int textX = 30;
    constexpr int textX_alt = 25;
    constexpr int textX_centered = 256 / 2 - 43;
    constexpr int textY = 30;
    constexpr int lineSpacing = 20;
    constexpr int characterExtraSpacing = 3;
    constexpr int characterExtraSpacing_intro3 = 2;
    constexpr int dotFrames = 40;
    constexpr int otherPunctuationFrames = 25;
    constexpr int letterFrames = 4;
    int timer;

    std::string textBuffer;
    FILE* textStream = fopen("nitro:/data/intro.txt", "rb");
    if (textStream == nullptr)
        nocashMessage("Error opening intro text");

    Engine::Background cBackground;
    std::string buffer;

    Engine::Font mainFont;
    mainFont.loadPath("fnt_maintext.font");

    Audio2::playBGMusic("mus_story_mod.wav", true);

    setBrightness(1, -16);
    bool skip = false;

    for (int introIdx = 0; introIdx < 11 && !skip; introIdx++) {
        buffer = "intro/intro" + std::to_string(introIdx);
        cBackground.loadPath(buffer);

        cBackground.loadBgTextMain();
        if (introIdx == 10)  // Intro last has scrolling
            REG_BG3VOFS = height-192;

        timer = fadeInFrames;
        while (timer >= 0 && introIdx != 0 && !skip) {
            Engine::tick();
            skip = keysDown() != 0;
            setBrightness(1, (-16 * timer) / fadeInFrames);
            timer--;
        }
        setBrightness(1, 0);

        if (introIdx == 10) {  // Intro last has longer hold
            timer = holdLastFrames;
        } else {
            timer = holdFrames;
        }

        int textTimer = letterFrames;

        if (textStream) {
            int textLen = str_len_file(textStream, '@');
            textBuffer.resize(textLen);
            fread(&textBuffer[0], textLen, 1, textStream);
            fseek(textStream, 2, SEEK_CUR); // read @\n characters
        } else {
            textBuffer = "";  // if file couldn't be opened don't write anything
        }

        const char* textPointer = textBuffer.c_str();
        int initialX = textX;
        if (introIdx == 3 || introIdx == 6)  // Fit to screen
            initialX = textX_alt;
        else if (introIdx == 5)  // MT EBOTT. centered
            initialX = textX_centered;
        int x = initialX, y = textY;
        Engine::textSub.clear();
        while (timer >= 0 && !skip) {
            Engine::tick();
            skip = keysDown() != 0;
            if (textTimer == 0 && *textPointer != 0) {
                char glyph = *textPointer++;
                if (glyph == '\n') {
                    x = initialX;
                    y += lineSpacing;
                } else {
                    Engine::textSub.drawGlyph(mainFont, glyph, x, y);
                    if (introIdx == 3)  // Fit to screen
                        x += characterExtraSpacing_intro3;
                    else
                        x += characterExtraSpacing;
                }
                textTimer = letterFrames;
                if (glyph == '.' && introIdx != 5)  // Don't want time on MT. EBOTT
                    textTimer = dotFrames;
                else if (glyph == ',' || glyph == ':' || glyph == ';')
                    textTimer = otherPunctuationFrames;
            }
            timer--;
            if (*textPointer != 0)
                textTimer--;
        }

        if (introIdx == 10) {  // Intro last scroll
            timer = scrollFrames;
            while (timer >= 0 && !skip) {
                Engine::tick();
                skip = keysDown() != 0;
                REG_BG3VOFS = ((height-192) * timer) / scrollFrames;
                timer--;
            }

            timer = holdLastFrames;
            while (timer >= 0 && !skip) {
                Engine::tick();
                skip = keysDown() != 0;
                timer--;
            }
        }

        if (introIdx == 10)  // Intro last has longer fade out
            fadeOutFrames = fadeOutLastFrames;  // Can change value as it's never used again
        timer = fadeOutFrames;
        while (timer >= 0 && !skip) {
            Engine::tick();
            skip = keysDown() != 0;
            setBrightness(1, (-16 * (fadeOutFrames - timer)) / fadeOutFrames);
            if (introIdx == 6)
                setBrightness(2, (-16 * (fadeOutFrames - timer)) / fadeOutFrames);
            timer--;
        }
    }

    Engine::textSub.clear();

    skip = false;
    fclose(textStream);
    Audio2::playBGMusic("mus_intronoise.wav", false);

    Engine::Background titleBottom;
    titleBottom.loadPath("intro/title_bottom");

    cBackground.loadPath("intro/title");

    cBackground.loadBgTextMain();

    REG_BG3VOFS = 0;
    setBrightness(3, 0);  // set brightness to full bright

    timer = introLogoFrames;

    while (!skip) {
        Engine::tick();
        skip = keysDown() != 0;
        if (timer > 0) {
            timer--;
            if (timer <= 0) {
                titleBottom.loadBgTextSub();
            }
        }
    }

    Engine::clearSub();

    Audio2::stopBGMusic();
}
