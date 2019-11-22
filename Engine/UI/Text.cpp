#include "../UI/Text.h"

#include "../Assets/Texture.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using CodeMonkeys::Engine::UI::Text;
using namespace std;


map<char, Texture*> Text::characters;

Text::Text(string text, vec2 start_position, float font_size) : Quad(vec2(0), vec2(font_size))
{
    this->text = text;
    this->start_position = start_position;
    this->font_size = font_size;
    this->character_spacing = 1;
    this->line_spacing = 1;

    if (Text::characters.size() == 0)
        Text::init();
}

void Text::init()
{
    vector<char> available_characters;
    for (unsigned int i = 'a'; i <= 'z'; i++)
    {
        available_characters.push_back((char)i);
    }
    for (unsigned int i = '0'; i <= '9'; i++)
    {
        available_characters.push_back((char)i);
    }
    available_characters.push_back('-');
    available_characters.push_back(':');
    available_characters.push_back('?');
    available_characters.push_back('!');
    available_characters.push_back('.');

    for (unsigned int i = 0; i < available_characters.size(); i++)
    {
        char character = available_characters[i];
        std::ostringstream ss;
        ss << "Assets/UI/Font/" << character << ".png";
        Texture* texture = new Texture(ss.str().c_str());
        Text::characters[character] = texture;
    }
}

string Text::get_text()
{
    return this->text;
}

void Text::set_text(string text)
{
    this->text = text;
}

vec2 Text::get_start_position()
{
    return this->start_position;
}

void Text::set_start_position(vec2 start_position)
{
    this->start_position = start_position;
}

float Text::get_font_size()
{
    return this->font_size;
}

void Text::set_font_size(float font_size)
{
    this->font_size = font_size;
    this->size = vec2(this->font_size, this->font_size);
}

float Text::get_character_spacing()
{
    return this->character_spacing;
}
void Text::set_character_spacing(float character_spacing)
{
    this->character_spacing = character_spacing;
}

float Text::get_line_spacing()
{
    return this->line_spacing;
}

void Text::set_line_spacing(float line_spacing)
{
    this->line_spacing = line_spacing;
}

void Text::draw()
{
    vec2 current_position = this->start_position;
    for (unsigned int i = 0; i < this->text.size(); i++)
    {
        if (this->text[i] == '\n')
        {
            current_position.x = this->start_position.x;
            current_position.y -= this->font_size * this->line_spacing;
        }
        else
        {
            this->draw_character(this->text[i], current_position);
            current_position.x += this->font_size * this->character_spacing;
        }
    }
}

void Text::draw_character(char character, vec2 character_position)
{
    if (character == ' ')
        return;

    this->position = character_position;
    this->set_texture(Text::characters[character]->get_texture_id());
    Quad::draw();
}