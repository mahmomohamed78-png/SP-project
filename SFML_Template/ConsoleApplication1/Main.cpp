#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <fstream>
#include <SFML/Audio.hpp>
#include <iomanip>
using namespace std;
using namespace sf;



int currentwindow = 0;
int pausedlevel = 0;
int wonlevel = 0;

bool levelTimerActive = false;
float levelTimeElapsed = 0.f;

struct design
{
    Texture texture;
    Sprite sprite;

    int framecounter = 0;
    float timer = 0;
    bool opened = false;
};


struct character {
    const int frameWidth = 160;
    int frameHeight; //boy 140, girl 128 

    Sprite sprite;
    void sprite_origin() {
        sprite.setTextureRect(IntRect(0, 0, frameWidth, frameHeight));
        sprite.setOrigin(sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2);
    }

    float speed_y;
    float speed_x;
    float gravity;
    float jump_strength;
    float delay;
    float timer;

    int framecounter;
    int totalFrames;

    Keyboard::Key left;
    Keyboard::Key right;
    Keyboard::Key up;

    bool onground;
    bool ismoving;
    bool stop;
    bool is_dead = false;
};

int selectedmmIndex = 0;
int selectedpmIndex = 0;

Text pause_txt[4];
String pausetxt[] = { "R e s u m e","B a c k   t o   m a i n   m e n u","B a c k   t o   l e v e l s   m e n u","s o u n d s" };
Text main_txt[5];
String maintxt[] = { "t o   l e v e l   m e n u","c r e d i t","c o n t r o l s","s o u n d s","e x i t" };

//////////////////////////////modification(2)/////////////////////////////////////////////////
// Sound Menu Variables
bool isMenuMusicOn = true;
bool isGameMusicOn = true;

Text sound_title;
RectangleShape sound_btnBoxes[3];
Text sound_txt[3];
String soundtxt[] = { "Main Menu Music : ON", "Levels Music : ON", "Back to Main Menu" };
//////////////////////////////////////////////////////////////////////////////////////////////
void MoveSelection(int direction, Color newColor, Color oldColor)

{
    //main menu
    switch (currentwindow)
    {
    case 0:

        main_txt[selectedmmIndex].setFillColor(oldColor);
        selectedmmIndex += direction;
        if (selectedmmIndex < 0)selectedmmIndex = 4;
        else if (selectedmmIndex > 4)selectedmmIndex = 0;
        main_txt[selectedmmIndex].setFillColor(newColor);

        break;

    case 7:

        pause_txt[selectedpmIndex].setFillColor(oldColor);
        selectedpmIndex += direction;
        if (selectedpmIndex < 0)selectedpmIndex = 3;
        else if (selectedpmIndex > 3)selectedpmIndex = 0;
        pause_txt[selectedpmIndex].setFillColor(newColor);

        break;

    default:
        break;
    }
    
}


void moveCharacter(character& player, float deltaTime)
{
    player.ismoving = false;
    if (player.is_dead) return;
    if (Keyboard::isKeyPressed(player.left)) {

        player.ismoving = true;
        player.sprite.move(-player.speed_x * deltaTime, 0);
        player.sprite.setScale(-1, 1);

    }
    else if (Keyboard::isKeyPressed(player.right)) {

        player.ismoving = true;
        player.sprite.move(player.speed_x * deltaTime, 0);
        player.sprite.setScale(1, 1);
    }
}
void jumpCharacter(character& c, float deltaTime)
{
    if (!c.is_dead && Keyboard::isKeyPressed(c.up) && c.onground) {
        c.speed_y = c.jump_strength;
        c.onground = false;
    }
    if (!c.onground) {
        c.speed_y += c.gravity * deltaTime;
        c.sprite.move(0, c.speed_y * deltaTime);
    }
}
void animation(character& c, float deltaTime)
{
    if (!c.ismoving) {
        c.framecounter = 0;
        c.sprite.setTextureRect(IntRect(0, 0, c.frameWidth, c.frameHeight));
        return;
    }
    c.timer += deltaTime;
    if (c.timer >= c.delay) {
        c.timer = 0;
        c.framecounter++;
    }
    if (c.framecounter >= c.totalFrames) {
        c.framecounter = 1;
    }
    c.sprite.setTextureRect(
        IntRect(c.framecounter * c.frameWidth, 0, c.frameWidth, c.frameHeight)
    );

}

void platform_collision(character& player, RectangleShape& ground) {
    FloatRect hitbox = player.sprite.getGlobalBounds();
    hitbox.width = 40;
    hitbox.left += 65;
    if (hitbox.intersects(ground.getGlobalBounds())) {
        float playerBottom = hitbox.top + hitbox.height;
        float platformTop = ground.getPosition().y;
        if (player.speed_y > 0 && playerBottom <= platformTop + 30.0f) {
            player.sprite.setPosition(player.sprite.getPosition().x, platformTop - (player.frameHeight / 2.0));
            player.speed_y = 0;
            player.onground = true;
        }
        if (player.speed_y < 0 && hitbox.top >= platformTop - 30.0f) {
            player.sprite.setPosition(player.sprite.getPosition().x, platformTop + (ground.getSize().y) + (player.frameHeight / 2.0));
            player.speed_y = 0;
        }
    }
}


int smoke_framecounter = 0;
void died(character& player, design& smoke, float& dt, Sound& deathSound, Music& gameMusic) {
    if (!player.is_dead)
    {
        gameMusic.stop();
        deathSound.play();
    }
    player.stop = 1;
    player.is_dead = true;
    player.timer += dt;
    smoke.sprite.setPosition(player.sprite.getPosition().x - 84, player.sprite.getPosition().y - 150);
    if (player.is_dead && player.timer >= 0.1f) {
        smoke.sprite.setTextureRect(IntRect(smoke_framecounter * 168, 0, 168, 186));
        player.timer = 0;
        smoke_framecounter++;
    }
    if (smoke_framecounter >= 5) {
        smoke.sprite.setTextureRect(IntRect(5 * 168, 0, 168, 186));
    }
    if (player.stop) {
        player.speed_x = 0;
    }
    else {
        player.speed_x = 450.0f;
    }
}

void lakes_collision(character& no_died, character& yes_died, RectangleShape& lake, design& smoke, float& dt, Sound& deathSound, Music& gameMusic) {
    FloatRect hitbox = no_died.sprite.getGlobalBounds();
    hitbox.width = 40;
    hitbox.left += 55;
    FloatRect hitbox2 = yes_died.sprite.getGlobalBounds();
    hitbox2.width = 40;
    hitbox2.left += 55;
    if (hitbox.intersects(lake.getGlobalBounds())) {
        float playerBottom = hitbox.top + hitbox.height;
        float platformTop = lake.getPosition().y;
        if (no_died.speed_y > 0 && playerBottom <= platformTop + 15.0f) {
            no_died.sprite.setPosition(no_died.sprite.getPosition().x, platformTop - (no_died.frameHeight / 2.0));
            no_died.speed_y = 0;
            no_died.onground = true;
        }
        if (no_died.speed_y < 0 && hitbox.top >= platformTop - 15.0f) {
            no_died.sprite.setPosition(no_died.sprite.getPosition().x, platformTop + (lake.getSize().y) + (no_died.frameHeight / 2.0));
            no_died.speed_y = 0;
        }
    }
    if (hitbox2.intersects(lake.getGlobalBounds())) {
        float playerBottom2 = hitbox2.top + hitbox2.height;
        float platformTop2 = lake.getPosition().y;
        if (yes_died.speed_y > 0 && playerBottom2 <= platformTop2 + 15.0f) {
            died(yes_died, smoke, dt, deathSound, gameMusic);
            yes_died.sprite.setPosition(yes_died.sprite.getPosition().x, platformTop2 - (yes_died.frameHeight / 2.0));
            yes_died.speed_y = 0;
            yes_died.onground = true;

        }
        if (yes_died.speed_y < 0 && hitbox2.top >= platformTop2 - 15.0f) {
            yes_died.sprite.setPosition(yes_died.sprite.getPosition().x, platformTop2 + (lake.getSize().y) + (yes_died.frameHeight / 2.0));
            yes_died.speed_y = 0;
        }
    }
}


void acid_collision(character& fireBoy, character& waterGirl, RectangleShape& acid, design& smoke, float& dt, Sound& deathSound, Music& gameMusic) {
    FloatRect hitbox = waterGirl.sprite.getGlobalBounds();
    hitbox.width = 40;
    hitbox.left += 55;
    FloatRect hitbox2 = fireBoy.sprite.getGlobalBounds();
    hitbox2.width = 40;
    hitbox2.left += 55;
    if (hitbox.intersects(acid.getGlobalBounds())) {
        float playerBottom = hitbox.top + hitbox.height;
        float platformTop = acid.getPosition().y;
        if (waterGirl.speed_y > 0 && playerBottom <= platformTop + 15.0f) {
            died(waterGirl, smoke, dt, deathSound, gameMusic);
            waterGirl.sprite.setPosition(waterGirl.sprite.getPosition().x, platformTop - (waterGirl.frameHeight / 2.0));
            waterGirl.speed_y = 0;
            waterGirl.onground = true;
        }
        if (waterGirl.speed_y < 0 && hitbox.top >= platformTop - 15.0f) {
            waterGirl.sprite.setPosition(waterGirl.sprite.getPosition().x, platformTop + (acid.getSize().y) + (waterGirl.frameHeight / 2.0));
            waterGirl.speed_y = 0;
        }
    }
    if (hitbox2.intersects(acid.getGlobalBounds())) {
        float playerBottom2 = hitbox2.top + hitbox2.height;
        float platformTop2 = acid.getPosition().y;
        if (fireBoy.speed_y > 0 && playerBottom2 <= platformTop2 + 15.0f) {
            died(fireBoy, smoke, dt, deathSound, gameMusic);
            fireBoy.sprite.setPosition(fireBoy.sprite.getPosition().x, platformTop2 - (fireBoy.frameHeight / 2.0));
            fireBoy.speed_y = 0;
            fireBoy.onground = true;

        }
        if (fireBoy.speed_y < 0 && hitbox2.top >= platformTop2 - 15.0f) {
            fireBoy.sprite.setPosition(fireBoy.sprite.getPosition().x, platformTop2 + (acid.getSize().y) + (fireBoy.frameHeight / 2.0));
            fireBoy.speed_y = 0;
        }
    }
}



void slope_collision(character& player, double x1, double y1, double x2, double y2) {
    double player_x = player.sprite.getPosition().x;
    if (player_x >= x1 && player_x <= x2) {
        double slopeY = y1 + (player_x - x1) * ((y2 - y1) / (x2 - x1));
        double player_y = player.sprite.getPosition().y + (player.frameHeight / 2.0);
        if ((player_y >= slopeY - 5.0) && (player_y <= slopeY + 15.0)) {
            if (player.speed_y >= 0) {
                player.sprite.setPosition(player_x, slopeY - (player.frameHeight / 2.0f));
                player.speed_y = 0;
                player.onground = true;
            }
        }
    }
}

void snow_slope_collision(character& boy, character& girl, double x1, double y1, double x2, double y2, float& dt) {
    double boy_x = boy.sprite.getPosition().x;
    if (boy_x >= x1 && boy_x <= x2) {
        double boy_slopeY = y1 + (boy_x - x1) * ((y2 - y1) / (x2 - x1));
        double boy_y = boy.sprite.getPosition().y + (boy.frameHeight / 2.0);
        if ((boy_y >= boy_slopeY - 5.0) && (boy_y <= boy_slopeY + 15.0)) {
            if (boy.speed_y >= 0) {
                boy.sprite.setPosition(boy_x, boy_slopeY - (boy.frameHeight / 2.0f));
                boy.speed_y = 0;
                boy.jump_strength = -500;
                boy.onground = true;
                boy.sprite.move(-450 * 1.1 * dt, boy.speed_y * dt);
            }
        }
    }
    else {
        boy.jump_strength = -950;
    }
    double girl_x = girl.sprite.getPosition().x;
    if (girl_x >= x1 && girl_x <= x2) {
        double girl_slopeY = y1 + (girl_x - x1) * ((y2 - y1) / (x2 - x1));
        double girl_y = girl.sprite.getPosition().y + (girl.frameHeight / 2.0);
        if ((girl_y >= girl_slopeY - 5.0) && (girl_y <= girl_slopeY + 15.0)) {
            if (girl.speed_y >= 0) {
                girl.sprite.setPosition(girl_x, girl_slopeY - (girl.frameHeight / 2.0f));
                girl.speed_y = 0;
                girl.speed_x = girl.speed_x * 0.2;
                girl.onground = true;
            }
        }
    }
}

void wall_collision(character& boy, character& girl, RectangleShape& wall) {
    FloatRect hitbox1 = boy.sprite.getGlobalBounds();
    hitbox1.width = 70;
    hitbox1.left += 45;
    hitbox1.height -= 10.0f;
    hitbox1.top += 5.0f;
    if (hitbox1.intersects(wall.getGlobalBounds())) {
        if (hitbox1.left < wall.getPosition().x && boy.sprite.getScale().x == 1) {
            boy.stop = true;
        }
        else if (hitbox1.left > wall.getPosition().x && boy.sprite.getScale().x == -1) {
            boy.stop = true;
        }
        else {
            boy.stop = false;
        }
    }

    if (boy.stop) {
        boy.speed_x = 0;
    }
    else {
        boy.speed_x = 450.0f;
    }

    FloatRect hitbox2 = girl.sprite.getGlobalBounds();
    hitbox2.width = 90;
    hitbox2.left += 35;
    hitbox2.height -= 10.0f;
    hitbox2.top += 5.0f;
    if (hitbox2.intersects(wall.getGlobalBounds())) {
        if (hitbox2.left < wall.getPosition().x && girl.sprite.getScale().x == 1) {
            girl.stop = true;
        }
        else if (hitbox2.left > wall.getPosition().x && girl.sprite.getScale().x == -1) {
            girl.stop = true;
        }
        else {
            girl.stop = false;
        }
    }

    if (girl.stop) {
        girl.speed_x = 0;
    }
    else {
        girl.speed_x = 450.0f;
    }
}



void door_collision(character& player, design& door, float dt) {
    FloatRect hitbox = player.sprite.getGlobalBounds();
    hitbox.width = 40;
    hitbox.left += 55;
    door.opened = hitbox.intersects(door.sprite.getGlobalBounds());
    door.timer += dt;
    if (door.timer >= 0.16f) {
        door.timer = 0;
        if (door.opened) {
            if (door.framecounter < 6) {
                door.framecounter++;
                door.sprite.setTextureRect(IntRect(door.framecounter * 121, 0, 121, 144));
            }
        }
        else {
            if (door.framecounter > 0) {
                door.framecounter--;
                door.sprite.setTextureRect(IntRect(door.framecounter * 121, 0, 121, 144));
            }
        }
    }
}

//initialize score counter
int points_counter = 0;
int fireboy_score = 0;
int watergirl_score = 0;


int highScore_fire[3] = { 0, 0, 0 };    // high score ??? level
int highScore_water[3] = { 0, 0, 0 };
float bestTime[3] = { 999.f, 999.f, 999.f }; // ??? ??? ??? level


void point_collision(character& player1, character& player2, design& point, int& score, Sound& collectSound, Sound& wrongCollectSound) {
    FloatRect hitbox = player1.sprite.getGlobalBounds();
    hitbox.width = 40;
    hitbox.left += 55;
    FloatRect hitbox2 = player2.sprite.getGlobalBounds();
    hitbox2.width = 40;
    hitbox2.left += 55;
    if (hitbox.intersects(point.sprite.getGlobalBounds())) {
        if (point.sprite.getColor().a != 0) {
            score++;
            points_counter++;
            collectSound.play();
            point.sprite.setColor(Color(255, 255, 255, 0));
        }
    }
    else if (hitbox2.intersects(point.sprite.getGlobalBounds())) {
        if (point.sprite.getColor().a != 0) {
            wrongCollectSound.play();
            point.sprite.setColor(Color(255, 255, 255, 0));
        }
    }
}

struct animation_lakes
{
    Texture texture;
    Sprite sprite;
    const int frameWidth = 128;
    const int frameHeight = 32;
    int framecounter = 0;
    const int totalFrames = 4;
    float delay = 0.25f;
    float timer = 0.0f;

};

void lake_animation(animation_lakes& lake, float deltaTime) {
    lake.timer += deltaTime;
    if (lake.timer >= lake.delay) {
        lake.timer = 0;
        lake.framecounter++;
    }
    if (lake.framecounter >= lake.totalFrames) {
        lake.framecounter = 0;
    }
    lake.sprite.setTextureRect(IntRect(lake.framecounter * lake.frameWidth, 0, lake.frameWidth, lake.frameHeight));
}

void origin(RectangleShape& x)
{
    x.setOrigin(x.getLocalBounds().width / 2, x.getLocalBounds().height / 2);
}

design Frame;
design background;

int main()
{
    //sounds
    //sound menu
    Music menuMusic;
    if (!menuMusic.openFromFile("gamesounds/main_menusound.wav"))
    {
        cout << "error loading menu music\n";
    }
    menuMusic.setLoop(true);
    menuMusic.setVolume(40);
    menuMusic.play();
    //sound levels
    Music gameMusic;
    if (!gameMusic.openFromFile("gamesounds/teast_gamesound2.ogg"))
    {
        cout << "error loading game music\n";
    }
    gameMusic.setLoop(true);
    gameMusic.setVolume(35);
    //sound death
    SoundBuffer deathBuffer;
    if (!deathBuffer.loadFromFile("gamesounds/testdie3.wav"))
    {
        cout << "error loading death sound\n";
    }
    Sound deathSound;
    deathSound.setBuffer(deathBuffer);
    deathSound.setVolume(60);
    //sound game over
    SoundBuffer gameOverBuffer;
    if (!gameOverBuffer.loadFromFile("gamesounds/gameover2.wav"))
    {
        cout << "error loading game over sound\n";
    }

    Sound gameOverSound;
    gameOverSound.setBuffer(gameOverBuffer);
    gameOverSound.setVolume(70);

    bool gameOverSoundPlayed = false;
    // sound collect
    SoundBuffer collectBuffer;
    if (!collectBuffer.loadFromFile("gamesounds/collect2.wav"))
    {
        cout << "error loading collect sound\n";
    }

    Sound collectSound;
    collectSound.setBuffer(collectBuffer);
    collectSound.setVolume(50);
    // sound win
    SoundBuffer winBuffer;
    if (!winBuffer.loadFromFile("gamesounds/win2.wav"))
    {
        cout << "error loading win sound\n";
    }

    Sound winSound;
    winSound.setBuffer(winBuffer);
    winSound.setVolume(70);

    bool winSoundPlayed = false;
    // wrong collect sound
    SoundBuffer wrongCollectBuffer;
    if (!wrongCollectBuffer.loadFromFile("gamesounds/missCollect.wav"))
    {
        cout << "error loading wrong collect sound\n";
    }

    Sound wrongCollectSound;
    wrongCollectSound.setBuffer(wrongCollectBuffer);
    wrongCollectSound.setVolume(50);



    //definitions level1
    design ground[12];
    design Platform[2];
    design fire_lake[3];
    design water_lake[2];
    animation_lakes fire[3];
    animation_lakes water[2];
    design triangle;
    design fire_point[3];
    design water_point[3];
    RectangleShape collision_boxs[12];
    RectangleShape walls[4];
    RectangleShape lakes[5];
    design smoke;
    design door[2];
    RectangleShape rec[135];



    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Level 2 definitions
    design background2;

    design ground2[10];
    design platform2[3];

    design fire_lake2[2];
    design water_lake2[2];
    design green_lake2[2];

    animation_lakes water2[2];
    animation_lakes fire2[2];
    animation_lakes acid2[2];

    design fire_point2[8];
    design water_point2[8];

    design slope_left2;
    design slope_right2;
    design right_corner2;
    design step_corner2;

    RectangleShape collision_boxs2[20];
    RectangleShape lakes2[6];
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Level 3 definitions

    design ground_3[14];

    design coloumn_3[4];

    design fire_lake3;

    design corner_3;

    design SNOW_3;

    design green_lake_3;

    animation_lakes fire3;
    animation_lakes acid3;

    design fire_point3[5];
    design water_point3[5];

    design stairs[5];
    //////////////////////////////modification/////////////////////////////////////////////////
    design button;
    RectangleShape button_collision;
    design lift;
    RectangleShape lift_collision;
    ///////////////////////////////////////////////////////////////////////////////////////////

    RectangleShape collision_boxs3[26];
    RectangleShape lakes3[2];
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //load level1
    Frame.texture.loadFromFile("game_textures/sprite_frame_level0/sprite_frame_level2.20.png");
    background.texture.loadFromFile("game_textures/background/Gemini_Generated_Image_pn65sypn65sypn65.png");
    smoke.texture.loadFromFile("game_textures\\smoke2.0.png");
    ground[0].texture.loadFromFile("game_textures\\Tile_56\\sprite_colom0.png");
    ground[1].texture.loadFromFile("game_textures/Tile_02/sprite_ground2.20.png");
    ground[2].texture.loadFromFile("game_textures/Tile_02/sprite_ground2.20.png");
    ground[3].texture.loadFromFile("game_textures/hight_grounds/hight_grounds/sprite.png");
    ground[4].texture.loadFromFile("game_textures\\hight_grounds\\hight_grounds\\Tile_56_1_1.png");
    ground[5].texture.loadFromFile("game_textures\\New_Piskel_14\\New_Piskel_14.png");
    ground[6].texture.loadFromFile("game_textures\\Tile_03.png");
    ground[7].texture.loadFromFile("game_textures\\hight_grounds\\hight_grounds\\Tile_56_1_1.png");
    ground[8].texture.loadFromFile("game_textures/hight_grounds/hight_grounds/sprite.png");
    ground[9].texture.loadFromFile("game_textures/hight_grounds/hight_grounds/sprite.png");
    ground[10].texture.loadFromFile("game_textures\\hight_grounds\\hight_grounds\\Tile_56_1_1.png");
    ground[11].texture.loadFromFile("game_textures/hight_grounds/hight_grounds/sprite.png");
    Platform[0].texture.loadFromFile("game_textures\\Platform\\sprite_platform2.00.png");
    Platform[1].texture.loadFromFile("game_textures\\Platform\\sprite_platform2.00.png");
    fire_lake[0].texture.loadFromFile("game_textures/sprite_01/sprite_0.png");
    fire_lake[1].texture.loadFromFile("game_textures\\hight_grounds\\hight_grounds\\sprite_0_1.png");
    fire_lake[2].texture.loadFromFile("game_textures\\hight_grounds\\hight_grounds\\sprite_0_1.png");
    water_lake[0].texture.loadFromFile("game_textures\\hight_grounds\\hight_grounds\\6375e845-48fe-4c20-ad50-a5e0cc2a226d_1.png");
    water_lake[1].texture.loadFromFile("game_textures\\hight_grounds\\hight_grounds\\6375e845-48fe-4c20-ad50-a5e0cc2a226d_1.png");
    triangle.texture.loadFromFile("game_textures\\Triangle\\sprite_tri0.png");
    fire_point[0].texture.loadFromFile("game_textures\\Water_Spell_Frame_03\\Icons_Fire Spell.png");
    fire_point[1].texture.loadFromFile("game_textures\\Water_Spell_Frame_03\\Icons_Fire Spell.png");
    fire_point[2].texture.loadFromFile("game_textures\\Water_Spell_Frame_03\\Icons_Fire Spell.png");
    water_point[0].texture.loadFromFile("game_textures\\Water_Spell_Frame_03\\Water_Spell_Frame_03.png");
    water_point[1].texture.loadFromFile("game_textures\\Water_Spell_Frame_03\\Water_Spell_Frame_03.png");
    water_point[2].texture.loadFromFile("game_textures\\Water_Spell_Frame_03\\Water_Spell_Frame_03.png");
    door[0].texture.loadFromFile("game_textures\\fireboyenddoor.png");
    door[1].texture.loadFromFile("game_textures\\watergirlenddoor.png");
    fire[0].texture.loadFromFile("game_textures\\big_lava_Animation.png");
    for (int i = 1; i < 3; i++)
        fire[i].texture.loadFromFile("game_textures\\lava_Animation.png");
    for (int i = 0; i < 2; i++)
        water[i].texture.loadFromFile("game_textures\\water_Animation.png");

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
    // 
// Load level 2 textures

    background2.texture.loadFromFile("game_textures/background/Gemini_Generated_Image_pn65sypn65sypn65.png");

    for (int i = 0; i < 10; i++)
        ground2[i].texture.loadFromFile("game_textures/hight_grounds/hight_grounds/Tile_56_1_1.png");

    for (int i = 0; i < 3; i++)
        platform2[i].texture.loadFromFile("game_textures/Platform/sprite_platform2.00.png");

    for (int i = 0; i < 2; i++)
        fire_lake2[i].texture.loadFromFile("game_textures/hight_grounds/hight_grounds/sprite_0_1.png");

    for (int i = 0; i < 2; i++)
        water_lake2[i].texture.loadFromFile("game_textures/hight_grounds/hight_grounds/6375e845-48fe-4c20-ad50-a5e0cc2a226d_1.png");

    for (int i = 0; i < 2; i++)
        green_lake2[i].texture.loadFromFile("acid_lake/208910c6-0852-4eec-bdad-457062911bb6.png");

    for (int i = 0; i < 8; i++)
        fire_point2[i].texture.loadFromFile("game_textures/Water_Spell_Frame_03/Icons_Fire Spell.png");

    for (int i = 0; i < 8; i++)
        water_point2[i].texture.loadFromFile("game_textures/Water_Spell_Frame_03/Water_Spell_Frame_03.png");

    for (int i = 0; i < 2; i++) {
        water2[i].texture.loadFromFile("game_textures\\water_Animation.png");
        fire2[i].texture.loadFromFile("game_textures\\lava_Animation.png");
        acid2[i].texture.loadFromFile("game_textures\\Acid_Animation.png");
    }

    slope_left2.texture.loadFromFile("leveel2/Tile_48_1.png");
    slope_right2.texture.loadFromFile("leveel2/Tile_48_2.png");
    right_corner2.texture.loadFromFile("leveel2/Tile_48_3.png");

    step_corner2.texture.loadFromFile("leveel2/Tile_48_4.png");

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Load level 3 textures
    for (int i = 0; i < 14; i++) {
        ground_3[i].texture.loadFromFile("game_textures/hight_grounds/hight_grounds/Tile_56_1_1.png");
    }

    for (int i = 0; i < 4; i++) {
        coloumn_3[i].texture.loadFromFile("game_textures\\Tile_56\\sprite_colom0.png");
    }

    fire_lake3.texture.loadFromFile("game_textures/hight_grounds/hight_grounds/sprite_0_1.png");

    corner_3.texture.loadFromFile("leveel2/Tile_48_4.png");

    SNOW_3.texture.loadFromFile("game_textures/snow/sprite_snow0.png");

    green_lake_3.texture.loadFromFile("acid_lake/208910c6-0852-4eec-bdad-457062911bb6.png");

    for (int i = 0; i < 5; i++)
        fire_point3[i].texture.loadFromFile("game_textures/Water_Spell_Frame_03/Icons_Fire Spell.png");

    for (int i = 0; i < 5; i++)
        water_point3[i].texture.loadFromFile("game_textures/Water_Spell_Frame_03/Water_Spell_Frame_03.png");

    for (int i = 0; i < 5; i++)
        stairs[i].texture.loadFromFile("game_textures/Tile_03.png");

    fire3.texture.loadFromFile("game_textures\\lava_Animation.png");
    acid3.texture.loadFromFile("game_textures\\Acid_Animation.png");
    //////////////////////////////modification/////////////////////////////////////////////////
    button.texture.loadFromFile("game_textures\\button.png");
    lift.texture.loadFromFile("game_textures\\lift.png");

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //Sprite level1
    Frame.sprite.setTexture(Frame.texture);
    background.sprite.setTexture(background.texture);
    for (int i = 0; i < 12; i++)
        ground[i].sprite.setTexture(ground[i].texture);


    Platform[0].sprite.setTexture(Platform[0].texture);
    Platform[1].sprite.setTexture(Platform[1].texture);

    for (int i = 0; i < 3; i++)
        fire_lake[i].sprite.setTexture(fire_lake[i].texture);

    for (int i = 0; i < 2; i++)
        water_lake[i].sprite.setTexture(water_lake[i].texture);

    triangle.sprite.setTexture(triangle.texture);
    for (int i = 0; i < 3; i++)
        fire_point[i].sprite.setTexture(fire_point[i].texture);

    for (int i = 0; i < 3; i++)
        water_point[i].sprite.setTexture(water_point[i].texture);

    smoke.sprite.setTexture(smoke.texture);
    smoke.sprite.setTextureRect(IntRect(0, 0, 168, 186));

    for (int i = 0; i < 2; i++) {
        door[i].sprite.setTexture(door[i].texture);
        door[i].sprite.setTextureRect(IntRect(0, 0, 121, 144));
    }

    for (int i = 0; i < 3; i++)
        fire[i].sprite.setTexture(fire[i].texture);
    for (int i = 0; i < 2; i++)
        water[i].sprite.setTexture(water[i].texture);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// sprites for level 2

    background2.sprite.setTexture(background2.texture);

    for (int i = 0; i < 10; i++) {
        if (i == 4) {

        }
        else {
            ground2[i].sprite.setTexture(ground2[i].texture);
        }
    }

    for (int i = 0; i < 3; i++)
        platform2[i].sprite.setTexture(platform2[i].texture);

    for (int i = 0; i < 2; i++)
        fire_lake2[i].sprite.setTexture(fire_lake2[i].texture);

    for (int i = 0; i < 2; i++)
        water_lake2[i].sprite.setTexture(water_lake2[i].texture);

    for (int i = 0; i < 2; i++)
        green_lake2[i].sprite.setTexture(green_lake2[i].texture);

    for (int i = 0; i < 8; i++)
        fire_point2[i].sprite.setTexture(fire_point2[i].texture);

    for (int i = 0; i < 8; i++)
        water_point2[i].sprite.setTexture(water_point2[i].texture);

    for (int i = 0; i < 2; i++) {
        water2[i].sprite.setTexture(water2[i].texture);
        fire2[i].sprite.setTexture(fire2[i].texture);
        acid2[i].sprite.setTexture(acid2[i].texture);
    }

    slope_right2.sprite.setTexture(slope_right2.texture);
    right_corner2.sprite.setTexture(right_corner2.texture);


    step_corner2.sprite.setTexture(step_corner2.texture);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // sprites for level 3

    for (int i = 0; i < 14; i++) {
        ground_3[i].sprite.setTexture(ground_3[i].texture);
    }

    for (int i = 0; i < 4; i++) {
        coloumn_3[i].sprite.setTexture(coloumn_3[i].texture);
    }

    fire_lake3.sprite.setTexture(fire_lake3.texture);

    corner_3.sprite.setTexture(corner_3.texture);

    SNOW_3.sprite.setTexture(SNOW_3.texture);

    green_lake_3.sprite.setTexture(green_lake_3.texture);

    for (int i = 0; i < 5; i++)
        fire_point3[i].sprite.setTexture(fire_point3[i].texture);

    for (int i = 0; i < 5; i++)
        water_point3[i].sprite.setTexture(water_point3[i].texture);

    for (int i = 0; i < 5; i++)
        stairs[i].sprite.setTexture(stairs[i].texture);

    fire3.sprite.setTexture(fire3.texture);
    acid3.sprite.setTexture(acid3.texture);
    //////////////////////////////modification/////////////////////////////////////////////////
    button.sprite.setTexture(button.texture);
    lift.sprite.setTexture(lift.texture);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    for (int i = 0; i < 135; i++) {
        rec[i].setSize(Vector2f(4 + 2 * i, 1));
        rec[i].setPosition(1504 - i, 470 + i);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Credit Window
    Texture cr_background;
    cr_background.loadFromFile("game_textures\\resized_1920x1080.png");
    Sprite credit_BG;
    credit_BG.setTexture(cr_background);

    Font font;
    if (!font.loadFromFile("game_textures\\Copy_of_font.TTF")) {

        return -1;
    }
    Text namesarr[8];
    string names_arr[] = { "N o u r h a n   A s h r a f","M a l a k   A h m e d","N o u r   G a d","A y a   M o h a m e d","M o h a m e d   M a h m o u d","Z i a d   E k r a m y","Z i a d   T a r e k" ,"T o   r e t u r n   P r e s s   (  X  )" };
    for (int i = 0; i < 8; i++) {
        namesarr[i].setString(names_arr[i]);
        namesarr[i].setCharacterSize(37);
        namesarr[i].setFillColor(Color::White);
        namesarr[i].setOutlineThickness(3);
        namesarr[i].setOutlineColor(Color::Black);
        if (i == 7)namesarr[i].setPosition(650, 150 + i * 100);
        else namesarr[i].setPosition(700, 150 + i * 100);
        namesarr[i].setScale(2.0f, 2.0f);
        namesarr[i].setFont(font);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //pause menu
    Texture mn_menu;
    mn_menu.loadFromFile("game_textures\\main_menu_resized_1920x1080.png");
    Sprite menu_BG;
    menu_BG.setTexture(mn_menu);


    for (int i = 0; i < 4; i++) {
        pause_txt[i].setString(pausetxt[i]);
        pause_txt[i].setCharacterSize(30);
        pause_txt[i].setFillColor(Color::White);
        pause_txt[i].setOutlineThickness(3);
        pause_txt[i].setOutlineColor(Color::Red);
        if (i == 0 || i == 3)pause_txt[i].setPosition(800, 300 + i * 200);
        else  pause_txt[i].setPosition(620, 300 + i * 200);
        pause_txt[i].setScale(2.5f, 2.5f);
        pause_txt[i].setFont(font);
    }
    pause_txt[0].setFillColor(Color::Yellow);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //control menu
    Texture control_BG, F_boy_BG, W_Girl_BG;
    control_BG.loadFromFile("game_textures\\resized_1920x1080.jpg");
    F_boy_BG.loadFromFile("game_textures\\ChatGPT_f_controlsImage.png");
    W_Girl_BG.loadFromFile("game_textures\\ChatGPT_W_controlsImage.png");
    Sprite c_BG, F_BG, W_BG;
    c_BG.setTexture(control_BG);
    F_BG.setTexture(F_boy_BG);
    W_BG.setTexture(W_Girl_BG);

    W_BG.setScale(0.4f, 0.4f);
    F_BG.setScale(0.4f, 0.4f);

    W_BG.setPosition(205, 440);
    F_BG.setPosition(1135, 440);

    Text control_txt[2];
    String controltxt[] = { "Water Girl Controls","Fire Boy Controls" };
    for (int i = 0; i < 2; i++) {
        control_txt[i].setString(controltxt[i]);
        control_txt[i].setCharacterSize(27);
        control_txt[i].setFillColor(Color::Black);
        control_txt[i].setOutlineThickness(4);
        control_txt[i].setOutlineColor(Color::White);
        control_txt[i].setPosition(280 + i * 920, 350);
        control_txt[i].setScale(2.5f, 2.5f);
        control_txt[i].setFont(font);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //main menu
    Texture main_menu;
    main_menu.loadFromFile("game_textures\\ChatGPT_Image.png");
    Sprite menu_BackGground;
    menu_BackGground.setTexture(main_menu);
    menu_BackGground.setScale(1.15f, 1.15f);


    for (int i = 0; i < 5; i++) {
        main_txt[i].setString(maintxt[i]);
        main_txt[i].setCharacterSize(30.0f);
        main_txt[i].setFillColor(Color::Cyan);
        main_txt[i].setOutlineThickness(2.0f);
        main_txt[i].setOutlineColor(Color::Red);
        if (i == 0) main_txt[i].setPosition(670, 530 + i * 100);
        else main_txt[i].setPosition(830, 530 + i * 100);
        main_txt[i].setScale(3.0f, 2.5f);
        main_txt[i].setFont(font);
    }
    main_txt[0].setFillColor(Color::Yellow);

    //////////////////////////////modification(2)/////////////////////////////////////////////////

        // --- Sound Menu Design ---
    sound_title.setFont(font);
    sound_title.setString("SOUND SETTINGS");
    sound_title.setCharacterSize(100);
    sound_title.setFillColor(Color(255, 215, 0));//gold
    sound_title.setOutlineThickness(6);
    sound_title.setOutlineColor(Color(139, 0, 0));//dark red
    sound_title.setOrigin(sound_title.getLocalBounds().width / 2.0f, sound_title.getLocalBounds().height / 2.0f);
    sound_title.setPosition(1920 / 2.0f, 150);
    for (int i = 0; i < 3; i++) {

        sound_btnBoxes[i].setSize(Vector2f(750, 110));
        sound_btnBoxes[i].setFillColor(Color(0, 0, 0, 180));
        sound_btnBoxes[i].setOutlineThickness(4);
        sound_btnBoxes[i].setOutlineColor(Color::Cyan);
        sound_btnBoxes[i].setOrigin(sound_btnBoxes[i].getSize().x / 2.0f, sound_btnBoxes[i].getSize().y / 2.0f);
        sound_btnBoxes[i].setPosition(1920 / 2.0f, 400 + i * 180);

        sound_txt[i].setFont(font);
        sound_txt[i].setString(soundtxt[i]);
        sound_txt[i].setCharacterSize(45);

        if (i < 2) sound_txt[i].setFillColor(Color(80, 255, 100));// green ON
        else sound_txt[i].setFillColor(Color::White); //white RETURNE
        sound_txt[i].setOutlineThickness(3);
        sound_txt[i].setOutlineColor(Color::Black);


        sound_txt[i].setOrigin(sound_txt[i].getLocalBounds().width / 2.0f, sound_txt[i].getLocalBounds().height / 2.0f);
        sound_txt[i].setPosition(1920 / 2.0f, 400 + i * 180 - 15);
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        //GAME OVER :(

    Font cmfont;
    cmfont.loadFromFile("game_textures\\Blood_Hunter_TTF_Demo.ttf");

    Text back_from_GameOver[2];
    String backfromGameOver[] = { "Back to Main Menu","Back to Level Menu" };
    for (int i = 0; i < 2; i++)
    {
        back_from_GameOver[i].setString(backfromGameOver[i]);
        back_from_GameOver[i].setFont(font);
        back_from_GameOver[i].setCharacterSize(60);
        back_from_GameOver[i].setFillColor(Color::White);
        back_from_GameOver[i].setPosition(650, 680 + i * 100);
    }
    Text gameOverText;
    gameOverText.setFont(cmfont);
    gameOverText.setString("GAME OVER");
    gameOverText.setCharacterSize(250);
    gameOverText.setFillColor(Color::Red);
    gameOverText.setPosition(100, 100);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Levels Menu
    Texture levelsMenuTexture;

    levelsMenuTexture.loadFromFile("game_textures\\background\\Gemini_Generated_Image_pn65sypn65sypn65.png");
    Sprite levelsMenuBG;
    levelsMenuBG.setTexture(levelsMenuTexture);
    levelsMenuBG.setScale(1.875, 1.5);

    Text levelsTitle;
    levelsTitle.setFont(font);
    levelsTitle.setString("SELECT LEVEL");
    levelsTitle.setCharacterSize(100);
    levelsTitle.setFillColor(Color(255, 215, 0));
    levelsTitle.setOutlineThickness(6);
    levelsTitle.setOutlineColor(Color(139, 0, 0));

    levelsTitle.setOrigin(levelsTitle.getLocalBounds().width / 2.0f, levelsTitle.getLocalBounds().height / 2.0f);
    levelsTitle.setPosition(1920 / 2.0f, 150);

    RectangleShape levelCards[3];
    Text levelTexts[3];
    String lvlNames[] = { "LEVEL 1", "LEVEL 2","LEVEL 3" };

    for (int i = 0; i < 3; i++) {

        levelCards[i].setSize(Vector2f(350, 450));
        levelCards[i].setOrigin(levelCards[i].getSize().x / 2.0f, levelCards[i].getSize().y / 2.0f);
        levelCards[i].setPosition(350 + i * 620, 550);
        levelCards[i].setFillColor(Color(0, 0, 0, 180));
        levelCards[i].setOutlineThickness(5);
        levelCards[i].setOutlineColor(Color::Cyan);


        levelTexts[i].setFont(font);
        levelTexts[i].setString(lvlNames[i]);
        levelTexts[i].setCharacterSize(60);
        levelTexts[i].setFillColor(Color::White);
        levelTexts[i].setOutlineThickness(4);
        levelTexts[i].setOutlineColor(Color::Blue);
        levelTexts[i].setOrigin(levelTexts[i].getLocalBounds().width / 2.0f, levelTexts[i].getLocalBounds().height / 2.0f);
        levelTexts[i].setPosition(350 + i * 620, 550);
    }

    Text levelsBackText;
    levelsBackText.setFont(font);
    levelsBackText.setString("Back to Main Menu (Press B)");
    levelsBackText.setCharacterSize(40);
    levelsBackText.setFillColor(Color::White);
    levelsBackText.setOutlineThickness(3);
    levelsBackText.setOutlineColor(Color::Black);
    levelsBackText.setPosition(50, 950);

     /////////////////////////////////////////////////////////////////////////////////////////////////////

    // best score text
    Text highScoreTxt[3];
    Text bestTimeTxt[3];
    for (int i = 0; i < 3; i++) {
        // best score text
        highScoreTxt[i].setFont(font);
        highScoreTxt[i].setCharacterSize(28);
        highScoreTxt[i].setFillColor(Color::Yellow);
        highScoreTxt[i].setOutlineThickness(2);
        highScoreTxt[i].setOutlineColor(Color::Black);
        highScoreTxt[i].setPosition(200 + i * 620, 700);

        // best time text
        bestTimeTxt[i].setFont(font);
        bestTimeTxt[i].setCharacterSize(28);
        bestTimeTxt[i].setFillColor(Color::Cyan);
        bestTimeTxt[i].setOutlineThickness(2);
        bestTimeTxt[i].setOutlineColor(Color::Black);
        bestTimeTxt[i].setPosition(200 + i * 620, 760);
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////

    //  score  text
    Text scoreTxt[2];
    for (int i = 0; i < 2; i++) {
        scoreTxt[i].setFont(font);
        scoreTxt[i].setCharacterSize(40);
        scoreTxt[i].setOutlineThickness(3);
        scoreTxt[i].setOutlineColor(Color::Black);
    }
    scoreTxt[0].setFillColor(Color(255, 80, 80));
    scoreTxt[0].setPosition(50, 20);
    scoreTxt[1].setFillColor(Color(80, 180, 255));
    scoreTxt[1].setPosition(300, 20);

    // time text
    Text timerTxt;
    timerTxt.setFont(font);
    timerTxt.setCharacterSize(40);
    timerTxt.setFillColor(Color::White);
    timerTxt.setOutlineThickness(3);
    timerTxt.setOutlineColor(Color::Black);
    timerTxt.setPosition(870, 20);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // YOU WIN MENU DESIGN 
    Texture winBgTexture;
    winBgTexture.loadFromFile("game_textures\\win_BG.png");
    Sprite winMenuBG;
    winMenuBG.setTexture(winBgTexture);

    Font winfont;
    winfont.loadFromFile("game_textures\\AversaVectors.otf");


    Text winTitleShadow;
    winTitleShadow.setFont(winfont);
    winTitleShadow.setString("YOU WON  ( ; ");
    winTitleShadow.setCharacterSize(270);
    winTitleShadow.setFillColor(Color(0, 140, 130)); // Teal 
    winTitleShadow.setOrigin(winTitleShadow.getLocalBounds().width / 2.0f, winTitleShadow.getLocalBounds().height / 2.0f);
    winTitleShadow.setPosition((1920 / 2.0f) + 8, 200 + 8);

    Text winTitle;
    winTitle.setFont(winfont);
    winTitle.setString("YOU WON  ( ; ");
    winTitle.setCharacterSize(270);
    winTitle.setFillColor(Color(80, 255, 100));
    winTitle.setOutlineThickness(5);
    winTitle.setOutlineColor(Color::White);
    winTitle.setOrigin(winTitle.getLocalBounds().width / 2.0f, winTitle.getLocalBounds().height / 2.0f);
    winTitle.setPosition(1920 / 2.0f, 200);

    RectangleShape btnBoxes[2];
    Text winOptions[2];
    String winOptionsText[] = { "Next Level", "Back to Level Menu" };
    Color btnColors[] = { Color(255, 70, 110, 240), Color(0, 180, 240, 240) };
    for (int i = 0; i < 2; i++) {

        btnBoxes[i].setSize(Vector2f(650, 100));
        btnBoxes[i].setFillColor(btnColors[i]);
        btnBoxes[i].setOutlineThickness(4);
        btnBoxes[i].setOutlineColor(Color::White);
        btnBoxes[i].setOrigin(btnBoxes[i].getSize().x / 2.0f, btnBoxes[i].getSize().y / 2.0f);
        btnBoxes[i].setPosition(1920 / 2.0f, 610 + i * 150);

        winOptions[i].setFont(font);
        winOptions[i].setString(winOptionsText[i]);
        winOptions[i].setCharacterSize(45);
        winOptions[i].setFillColor(Color::White);

        winOptions[i].setOutlineThickness(2);
        winOptions[i].setOutlineColor(Color(40, 40, 50));
        winOptions[i].setOrigin(winOptions[i].getLocalBounds().width / 2.0f, winOptions[i].getLocalBounds().height / 2.0f);
        winOptions[i].setPosition(1920 / 2.0f, (610 + i * 150) - 10);
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    //Level 1 Design

    //Frame
    Frame.sprite.setScale(1.1, 1.5);
    Frame.sprite.setPosition(-96, -96 - 32);


    //background
    background.sprite.setScale(1.875, 1.5);




    //ground0
    ground[0].sprite.setScale(3, 1.2);
    ground[0].sprite.setPosition(200, 1000 - 691.2 + 100 - 80);


    //// ground1
    ground[1].sprite.setScale(0.875, 1.5);
    ground[1].sprite.setPosition(1220, 1000);


    // ground2
    ground[2].sprite.setPosition(0, 1000);
    ground[2].sprite.setScale(0.875, 1.5);


    // ground3
    ground[3].sprite.setPosition(1920 - 105.6 - 182, 700 + 10);


    // ground4
    ground[4].sprite.setScale(0.5, 1);
    ground[4].sprite.setPosition(296, 650 + 50 - 80);


    // ground5
    ground[5].sprite.setPosition(744 - 128.4, 650 - 64 + 50 - 80);
    ground[5].sprite.setScale(1.2, 2);


    //ground6
    ground[6].sprite.setScale(2, 2);
    ground[6].sprite.setPosition(1448 - 172.8, 650 + 50 - 80);


    //ground7
    ground[7].sprite.setScale(0.5, 1);
    ground[7].sprite.setPosition(296, 1000 - 691.2 + 100 - 80);


    //ground8
    ground[8].sprite.setPosition(1920 - 105.6 - 182, 450 - 90);


    //ground9
    ground[9].sprite.setPosition(506 - 50, 300 - 80);


    //ground10
    ground[10].sprite.setScale(0.5, 1);
    ground[10].sprite.setPosition(932.6, 300 - 80);


    //ground11
    ground[11].sprite.setScale(-1, 1);
    ground[11].sprite.setPosition(1545 + 20.6, 300 - 80);


    //Platform0
    Platform[0].sprite.setPosition(848, 1080 - 128 - 64 - 10);

    //platform1
    Platform[1].sprite.setPosition(1450, 850);
    Platform[1].sprite.setScale(1 / 1.5, 1);


    // Triengle
    triangle.sprite.setScale(2, 2);
    triangle.sprite.setPosition(1448 - 70, 500 + 50 - 80);


    // Fire lake 1 
    fire_lake[0].sprite.setPosition(700, 1000);
    fire_lake[0].sprite.setScale(1.82, 2);
    fire[0].sprite.setPosition(700, 1000);
    fire[0].sprite.setScale(4.095f, 3.0f);


    // Fire lake 2 
    fire_lake[1].sprite.setScale(0.7, 2);
    fire_lake[1].sprite.setPosition(1073.6, 620);
    fire[1].sprite.setScale(1.575, 2);
    fire[1].sprite.setPosition(1073.6, 620);


    // Fire lake 3
    fire_lake[2].sprite.setScale(0.7, 2);
    fire_lake[2].sprite.setPosition(1092.6, 220);
    fire[2].sprite.setScale(1.575, 2);
    fire[2].sprite.setPosition(1092.6, 220);


    // Water lake 1
    water_lake[0].sprite.setScale(0.7, 2);
    water_lake[0].sprite.setPosition(456, 620);
    water[0].sprite.setScale(1.575, 2);
    water[0].sprite.setPosition(456, 620);


    // Water lake 2
    water_lake[1].sprite.setScale(0.7, 2);
    water_lake[1].sprite.setPosition(731, 220);
    water[1].sprite.setScale(1.575, 2);
    water[1].sprite.setPosition(731, 220);


    //fire_point1
    fire_point[0].sprite.setScale(0.1, 0.1);
    fire_point[0].sprite.setPosition(940, 950);


    //fire_point2
    fire_point[1].sprite.setScale(0.1, 0.1);
    fire_point[1].sprite.setPosition(350, 700 - 55 - 90);


    //fire_point3
    fire_point[2].sprite.setScale(0.1, 0.1);
    fire_point[2].sprite.setPosition(456 + 137, 300 - 55 - 90);

    //water_point1
    water_point[0].sprite.setScale(0.1, 0.1);
    water_point[0].sprite.setPosition(950 - 10, 856 - 23 - 20);


    //water_point2
    water_point[1].sprite.setScale(0.1, 0.1);
    water_point[1].sprite.setPosition(865.6, 600 - 22 - 90);


    //water_point3
    water_point[2].sprite.setScale(0.1, 0.1);
    water_point[2].sprite.setPosition(932.6 + 80, 300 - 55 - 90);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Level 2 Design
// ==========================
    background2.sprite.setScale(1.875f, 1.5f);

    // frame
    Frame.sprite.setScale(1.1f, 1.5f);
    Frame.sprite.setPosition(-96, -128);

    // --------------------------
    // Ground
    // --------------------------

    // top left ground
    ground2[0].sprite.setScale(2.05f, 1.0f); //3.15
    ground2[0].sprite.setPosition(147 + 270, 235);

    // top right ground
    ground2[1].sprite.setScale(2.05f, 1.0f); //1.65
    ground2[1].sprite.setPosition(1116 + 133, 235);

    // left vertical block
    ground2[2].sprite.setScale(0.75f, 2.55f);
    ground2[2].sprite.setPosition(10 - 97, 501);

    // middle ground carrying green lakes
    ground2[3].sprite.setScale(0.85f, 1.0f); //0.59375
    ground2[3].sprite.setPosition(153 - 160, 600);

    ground2[8].sprite.setScale(0.85f, 1.0f);
    ground2[8].sprite.setPosition(850 - 87 + 35 - 125, 600);

    ground2[9].sprite.setScale(0.85f, 1.0f);
    ground2[9].sprite.setPosition(1670 - 87 - 236, 600);


    // long lower-middle ground
    ground2[4].sprite.setScale(5.0f, 0.90f);
    ground2[4].sprite.setPosition(15, 700);

    // bottom ground
    ground2[5].sprite.setScale(1.16f, 1.0f);
    ground2[5].sprite.setPosition(0, 1016);

    ground2[6].sprite.setScale(1.6f, 1.0f);
    ground2[6].sprite.setPosition(666.64, 1016);


    ground2[7].sprite.setScale(1.05f, 1.0f);
    ground2[7].sprite.setPosition(1465 + 7, 1016);

    // --------------------------
    // platforms
    // --------------------------
    platform2[0].sprite.setPosition(550 - 200, 465);
    platform2[0].sprite.setScale(1.08f, 1.0f);

    platform2[1].sprite.setPosition(1325 - 295, 465);
    platform2[1].sprite.setScale(1.08f, 1.0f);

    platform2[2].sprite.setPosition(1620, 740);
    platform2[2].sprite.setScale(0.7f, 1.0f);

    // --------------------------
    // Corners
    // --------------------------

    // top-right sloped piece
    slope_right2.sprite.setPosition(135, 422); //1830,355
    slope_right2.sprite.setScale(-0.85f, 1.0f);

    // upper-right step
    step_corner2.sprite.setPosition(538, 235);// 1630,183
    step_corner2.sprite.setScale(-2.05f, 1.0f); // 1.08,1.50

    //ground2[1].sprite.setScale(1.65f, 1.0f);
    //ground2[1].sprite.setPosition(1116, 185);

    // middle-right vertical corner
    right_corner2.sprite.setPosition(1770, 850 + 60);
    right_corner2.sprite.setScale(1.9f, 1.8f);//5.9,10.9


    // --------------------------
    // Lakes
    // --------------------------

    // green lakes in the middle
    green_lake2[0].sprite.setScale(1.42f, 2.0f);
    green_lake2[0].sprite.setPosition(265, 600);

    green_lake2[1].sprite.setScale(1.42f, 2.0f);
    green_lake2[1].sprite.setPosition(1260 - 87 - 231, 600);

    acid2[0].sprite.setScale(3.195f, 2.0f);
    acid2[0].sprite.setPosition(265, 600);
    acid2[1].sprite.setScale(3.195f, 2.0f);
    acid2[1].sprite.setPosition(1260 - 87 - 231, 600);

    // left lower water lake
    water_lake2[0].sprite.setScale(1.26f, 1.0f);
    water_lake2[0].sprite.setPosition(320 + 18, 840);
    water2[0].sprite.setScale(2.835f, 1.2f);
    water2[0].sprite.setPosition(320 + 18, 840);

    // right lower fire lake
    fire_lake2[0].sprite.setScale(1.26f, 1.0f);
    fire_lake2[0].sprite.setPosition(1110 + 36, 840);
    fire2[0].sprite.setScale(2.835f, 1.2f);
    fire2[0].sprite.setPosition(1110 + 36, 840);

    // bottom left fire lake
    fire_lake2[1].sprite.setScale(1.03f, 2.0f);
    fire_lake2[1].sprite.setPosition(370, 1016);
    fire2[1].sprite.setScale(2.3175f, 2.0f);
    fire2[1].sprite.setPosition(370, 1016);

    // bottom right water lake
    water_lake2[1].sprite.setScale(1.03f, 2.0f);
    water_lake2[1].sprite.setPosition(1176, 1016);
    water2[1].sprite.setScale(2.3175f, 2.0f);
    water2[1].sprite.setPosition(1176, 1016);

    // --------------------------
    // Points scale
    // --------------------------
    for (int i = 0; i < 8; i++)
    {
        fire_point2[i].sprite.setScale(0.09f, 0.09f);
        water_point2[i].sprite.setScale(0.1f, 0.1f);
    }
    // --------------------------
    // Middle pair above green lakes
    // --------------------------
    fire_point2[1].sprite.setPosition(1150, 110);
    water_point2[1].sprite.setPosition(1100, 100);

    // --------------------------
    // Lower-middle row
    // --------------------------
    water_point2[2].sprite.setPosition(450 - 87 + 30 - 128 - 50, 530);
    fire_point2[2].sprite.setPosition(450 - 87 + 30 - 128 + 408, 530);

    water_point2[3].sprite.setPosition(1260 - 87 - 231 + 408, 530);
    fire_point2[3].sprite.setPosition(1260 - 87 - 231 - 50, 530);

    // --------------------------
    // Above lower lakes
    // --------------------------
    water_point2[4].sprite.setPosition(430, 770);
    water_point2[5].sprite.setPosition(570, 770);

    fire_point2[4].sprite.setPosition(1210, 770);
    fire_point2[5].sprite.setPosition(1370, 770);

    // --------------------------
    // Near bottom lakes
    // --------------------------
    fire_point2[6].sprite.setPosition(470, 950);
    fire_point2[7].sprite.setPosition(520, 950);

    water_point2[6].sprite.setPosition(1270, 950);
    water_point2[7].sprite.setPosition(1320, 950);
    ////////////////////////////////////////////////////////////////////////////////////////////////////

// Level 3 Design
// ==========================

    // --------------------------
    // Ground
    // --------------------------

    //Level 3 Design

      // top right ground
    ground_3[0].sprite.setScale(0.95f, 0.80f);
    ground_3[0].sprite.setPosition(1800, 700 - 70);

    ground_3[1].sprite.setScale(0.8f, 0.70f);
    ground_3[1].sprite.setPosition(1480, 550 - 100);

    ground_3[2].sprite.setScale(2.25f, 0.60f);
    ground_3[2].sprite.setPosition(1500, 235);

    ground_3[3].sprite.setScale(1.10f, 0.70f);
    ground_3[3].sprite.setPosition(1350, 800 - 50);

    // bottom ground 

    ground_3[4].sprite.setScale(0.85f, 0.70f);
    ground_3[4].sprite.setPosition(1270, 980);

    ///////////////////////////////////////////////////

    ground_3[5].sprite.setScale(0.50f, 0.70f);
    ground_3[5].sprite.setPosition(1050, 850);

    //////////////////////////////////////////////////

    ground_3[6].sprite.setScale(0.50f, 0.70f);
    ground_3[6].sprite.setPosition(560, 900);

    ////////////////////////////////////////////////
    ground_3[7].sprite.setScale(0.50f, 0.70f);
    ground_3[7].sprite.setPosition(1100, 500);
    ////////////////////////////////////////////////
    ground_3[8].sprite.setScale(0.50f, 0.70f);
    ground_3[8].sprite.setPosition(820, 720);


    // Bottom left ground
    // --------------------------
    ground_3[13].sprite.setPosition(0, 1080 - 64);
    ground_3[13].sprite.setScale(1.875, 1);

    // ground under gates
    // --------------------------
    ground_3[12].sprite.setScale(1.24, 1);
    ground_3[12].sprite.setPosition(200, 1080 - 64 - 200);

    // upper left ground
    // --------------------------
    ground_3[11].sprite.setScale(0.5, 0.3);
    ground_3[11].sprite.setPosition(125, 150);

    // ground with lava
    // --------------------------
    ground_3[10].sprite.setScale(0.5, 1);
    ground_3[10].sprite.setPosition(200, 306);

    ground_3[9].sprite.setScale(0.5, 1);
    ground_3[9].sprite.setPosition(360 + 288, 306);

    // coloumn //
 ////////////////////////////////////////////////////

 /// right coloumn 

    coloumn_3[0].sprite.setScale(1.10f, 0.38f);
    coloumn_3[0].sprite.setPosition(1480, 235);


    coloumn_3[1].sprite.setScale(1.10f, 1.24f);
    coloumn_3[1].sprite.setPosition(1350, 40);

    // left coloumn
    // -------------
    coloumn_3[2].sprite.setPosition(200, 1080 - 64 - 200 - 510);

    // coloumn under gates
    // --------------------------
    coloumn_3[3].sprite.setScale(2, 1);
    coloumn_3[3].sprite.setPosition(350 + 200, 1080 - 64 - 200);

    //lakes
    // --------------------------
   // fire lake
    fire_lake3.sprite.setScale(1, 2);
    fire_lake3.sprite.setPosition(360, 306);
    fire3.sprite.setScale(2.25, 2);
    fire3.sprite.setPosition(360, 306);
    ////////////////////////////////////////////
    // acid lake 
    //-------------------
    green_lake_3.sprite.setScale(4.4f, 2.05f);
    green_lake_3.sprite.setPosition(410 + 200, 1030);
    acid3.sprite.setScale(9.0f, 2.5f);
    acid3.sprite.setPosition(410 + 200, 1030);
    //corner
    corner_3.sprite.setScale(-1.5, 2);
    corner_3.sprite.setPosition(1045, 177);

    ///////////////////////////////////////
    // snow 
    //-----------------------------
    SNOW_3.sprite.setScale(1.3f, 0.7f);
    SNOW_3.sprite.setPosition(1550, 900);

    // --------------------------
    // Points scale
    // --------------------------
    for (int i = 0; i < 5; i++)
    {
        fire_point3[i].sprite.setScale(0.09f, 0.09f);
        water_point3[i].sprite.setScale(0.1f, 0.1f);
    }
    // --------------------------

    // right 2 pairs 
    // -------------
    water_point3[0].sprite.setPosition(1600, 480 - 100);

    water_point3[1].sprite.setPosition(1850, 630 - 100);

    fire_point3[0].sprite.setPosition(1400, 300);

    fire_point3[1].sprite.setPosition(1400, 600);

    // left upper water
    // -------------
    water_point3[2].sprite.setPosition(100 + 75, 90);

    // bottom right pair
    // -----------------
    fire_point3[2].sprite.setPosition(1340 + 75, 890);
    water_point3[3].sprite.setPosition(1270 + 75, 880);

    // bottom left fire
    // -----------------
    fire_point3[3].sprite.setPosition(650, 850);

    // bottom middle pair
    // -----------------
    water_point3[4].sprite.setPosition(900, 650);
    fire_point3[4].sprite.setPosition(1100, 800);

    // stairs on left
    // -----------------
    for (int i = 0; i < 3; i++)
        stairs[i].sprite.setScale(2, 1);
    for (int i = 3; i < 5; i++)
        stairs[i].sprite.setScale(-2, 1);

    stairs[0].sprite.setPosition(15, 1080 - 200);
    stairs[1].sprite.setPosition(15, 1080 - 500);
    stairs[2].sprite.setPosition(15, 1080 - 800);
    stairs[3].sprite.setPosition(200, 1080 - 350);
    stairs[4].sprite.setPosition(200, 1080 - 650);


    ////////////////////////////////////////////////////////////////
    //////////////////////////////modification/////////////////////////////////////////////////
    button.sprite.setPosition(1490, 682);
    lift.sprite.setPosition(-20, 953);
    lift.sprite.setScale(2.0f, 1.3f);

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    // 
    // Collision boxes and walls for level 1

    for (int i = 0; i < 12; i++) {
        Vector2f vector(ground[i].sprite.getGlobalBounds().width, ground[i].sprite.getGlobalBounds().height);
        collision_boxs[i].setSize(vector);
        collision_boxs[i].setPosition(ground[i].sprite.getPosition());
    }
    collision_boxs[11].setPosition(ground[11].sprite.getPosition().x - ground[11].sprite.getGlobalBounds().width, ground[11].sprite.getPosition().y);
    collision_boxs[0].setSize(Vector2f(ground[0].sprite.getGlobalBounds().width, ground[0].sprite.getGlobalBounds().height));
    collision_boxs[0].setPosition(ground[0].sprite.getPosition().x, ground[0].sprite.getPosition().y);
    collision_boxs[5].setSize(Vector2f(ground[5].sprite.getGlobalBounds().width - 152, ground[5].sprite.getGlobalBounds().height));
    collision_boxs[5].setPosition(ground[5].sprite.getPosition().x + 76, ground[5].sprite.getPosition().y);
    collision_boxs[8].setSize(Vector2f(ground[8].sprite.getGlobalBounds().width - 64, ground[8].sprite.getGlobalBounds().height));
    collision_boxs[7].setSize(Vector2f(ground[7].sprite.getGlobalBounds().width + 92, ground[7].sprite.getGlobalBounds().height));
    collision_boxs[7].setPosition(ground[7].sprite.getPosition().x - 92, ground[7].sprite.getPosition().y);
    collision_boxs[8].setPosition(ground[8].sprite.getPosition().x + 64, ground[8].sprite.getPosition().y);
    collision_boxs[9].setSize(Vector2f(ground[9].sprite.getGlobalBounds().width - 64, ground[9].sprite.getGlobalBounds().height));
    collision_boxs[9].setPosition(ground[9].sprite.getPosition().x + 64, ground[9].sprite.getPosition().y);
    collision_boxs[3].setSize(Vector2f(ground[3].sprite.getGlobalBounds().width - 64, ground[3].sprite.getGlobalBounds().height));
    collision_boxs[3].setPosition(ground[3].sprite.getPosition().x + 64, ground[3].sprite.getPosition().y);
    collision_boxs[11].setSize(Vector2f(ground[11].sprite.getGlobalBounds().width - 64, ground[11].sprite.getGlobalBounds().height));


    walls[0].setSize(Vector2f(30, 1280));
    walls[0].setPosition(-14, -100);
    //walls[0].setOrigin(walls[0].getLocalBounds().width, 0);
    walls[1].setSize(Vector2f(30, 1280));
    walls[1].setPosition(1890 + 14, -100);
    walls[2].setSize(Vector2f(1920, 40));
    walls[2].setPosition(0, 0);
    walls[3].setSize(Vector2f(Platform[0].sprite.getGlobalBounds().width, Platform[0].sprite.getGlobalBounds().height)); // changing the position
    walls[3].setPosition(Platform[0].sprite.getPosition().x, Platform[0].sprite.getPosition().y);
    walls[3].setFillColor(Color::Blue);

    for (int i = 0; i < 2; i++) {
        Vector2f vect(water_lake[i].sprite.getGlobalBounds().width, water_lake[i].sprite.getGlobalBounds().height / 3);
        lakes[i].setSize(vect);
        lakes[i].setPosition(water_lake[i].sprite.getPosition().x, water_lake[i].sprite.getPosition().y + (water_lake[i].sprite.getGlobalBounds().height * 2 / 3));
    }
    for (int i = 0; i < 3; i++) {
        Vector2f vect(fire_lake[i].sprite.getGlobalBounds().width, fire_lake[i].sprite.getGlobalBounds().height / 3);
        lakes[i + 2].setSize(vect);
        lakes[i + 2].setPosition(fire_lake[i].sprite.getPosition().x, fire_lake[i].sprite.getPosition().y + (fire_lake[i].sprite.getGlobalBounds().height * 2 / 3));
    }

    lakes[2].setPosition(fire_lake[0].sprite.getPosition().x, fire_lake[0].sprite.getPosition().y + (fire_lake[0].sprite.getGlobalBounds().height / 3.5));


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Collision boxes and walls for level 2

    for (int i = 0; i < 10; i++) {
        if (i == 4) {

        }
        else {
            Vector2f vect(ground2[i].sprite.getGlobalBounds().width, ground2[i].sprite.getGlobalBounds().height);
            collision_boxs2[i].setSize(vect);
            collision_boxs2[i].setPosition(ground2[i].sprite.getPosition());
        }
    }
    collision_boxs2[1].setSize(Vector2f(ground2[1].sprite.getGlobalBounds().width + 35, ground2[1].sprite.getGlobalBounds().height));
    collision_boxs2[1].setFillColor(Color::Blue);
    collision_boxs2[2].setSize(Vector2f(ground2[2].sprite.getGlobalBounds().width - 20, ground2[2].sprite.getGlobalBounds().height - 20));
    collision_boxs2[2].setPosition(ground2[2].sprite.getPosition().x, ground2[2].sprite.getPosition().y + 17);
    collision_boxs2[2].setFillColor(Color::Cyan);
    collision_boxs2[10].setSize(Vector2f(platform2[0].sprite.getGlobalBounds().width, platform2[0].sprite.getGlobalBounds().height));
    collision_boxs2[10].setPosition(platform2[0].sprite.getPosition());
    collision_boxs2[11].setSize(Vector2f(platform2[1].sprite.getGlobalBounds().width, platform2[1].sprite.getGlobalBounds().height));
    collision_boxs2[11].setPosition(platform2[1].sprite.getPosition());
    collision_boxs2[12].setSize(Vector2f(step_corner2.sprite.getGlobalBounds().width - 130, step_corner2.sprite.getGlobalBounds().height / 3));
    collision_boxs2[12].setPosition(210, step_corner2.sprite.getPosition().y + 64);
    //collision_boxs2[13].setSize(Vector2f(right_corner2.sprite.getGlobalBounds().width, right_corner2.sprite.getGlobalBounds().height));
    collision_boxs2[13].setSize(Vector2f(150, 200));
    collision_boxs2[13].setPosition(right_corner2.sprite.getPosition());
    //collision_boxs2[14].setSize(Vector2f(step_corner2.sprite.getGlobalBounds().width - 50, step_corner2.sprite.getGlobalBounds().height - 96));
    //collision_boxs2[14].setPosition(step_corner2.sprite.getPosition().x + 50 , step_corner2.sprite.getPosition().y + 100 - 2);
    collision_boxs2[15].setSize(Vector2f(slope_right2.sprite.getGlobalBounds().width - 64 + 32, slope_right2.sprite.getGlobalBounds().height));
    collision_boxs2[15].setPosition(-23, slope_right2.sprite.getPosition().y);
    //collision_boxs2[16].setSize(Vector2f(right_corner2.sprite.getGlobalBounds().width, right_corner2.sprite.getGlobalBounds().height));
    //collision_boxs2[16].setPosition(right_corner2.sprite.getPosition().x + 15, right_corner2.sprite.getPosition().y);
    collision_boxs2[16].setPosition(1895 + 9, 0); //right wall 
    collision_boxs2[16].setSize(Vector2f(50, 1080)); //right wall
    collision_boxs2[16].setFillColor(Color::Green);
    //walls[1].setSize(Vector2f(20, 1080));
    //walls[1].setPosition(1890 + 50, 0);
    //collision_boxs2[17].setSize(Vector2f(ground2[2].sprite.getGlobalBounds().width, ground2[2].sprite.getGlobalBounds().height)); //left wall
    //collision_boxs2[17].setPosition(ground2[2].sprite.getPosition()); //left wall
    collision_boxs2[17].setSize(Vector2f(21, 1080)); //left wall
    collision_boxs2[17].setPosition(-5, 0); //left wall
    collision_boxs2[17].setFillColor(Color::Red);
    collision_boxs2[18].setSize(Vector2f(slope_right2.sprite.getGlobalBounds().width, slope_right2.sprite.getGlobalBounds().height - 64));
    collision_boxs2[18].setPosition(slope_right2.sprite.getPosition().x, slope_right2.sprite.getPosition().y + 64);
    collision_boxs2[19].setSize(Vector2f(slope_right2.sprite.getGlobalBounds().width, slope_right2.sprite.getGlobalBounds().height));
    collision_boxs2[19].setPosition(slope_right2.sprite.getPosition().x, slope_right2.sprite.getPosition().y);
    collision_boxs2[4].setFillColor(Color::Black);



    for (int i = 0; i < 2; i++) {
        Vector2f vect(water_lake2[i].sprite.getGlobalBounds().width, water_lake2[i].sprite.getGlobalBounds().height / 3);
        lakes2[i].setSize(vect);
        lakes2[i].setPosition(water_lake2[i].sprite.getPosition().x, water_lake2[i].sprite.getPosition().y + (water_lake2[i].sprite.getGlobalBounds().height * 2 / 3));
        Vector2f vect2(green_lake2[i].sprite.getGlobalBounds().width, green_lake2[i].sprite.getGlobalBounds().height / 3);
        lakes2[i + 2].setSize(vect2);
        lakes2[i + 2].setPosition(green_lake2[i].sprite.getPosition().x, green_lake2[i].sprite.getPosition().y + (green_lake2[i].sprite.getGlobalBounds().height * 2 / 3));
        Vector2f vect3(fire_lake2[i].sprite.getGlobalBounds().width, fire_lake2[i].sprite.getGlobalBounds().height / 3);
        lakes2[i + 4].setSize(vect3);
        lakes2[i + 4].setPosition(fire_lake2[i].sprite.getPosition().x, fire_lake2[i].sprite.getPosition().y + (fire_lake2[i].sprite.getGlobalBounds().height * 2 / 3));
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Collision boxes and walls for level 3
    for (int i = 0; i < 14; i++) {
        Vector2f vect(ground_3[i].sprite.getGlobalBounds().width, ground_3[i].sprite.getGlobalBounds().height);
        collision_boxs3[i].setSize(vect);
        collision_boxs3[i].setPosition(ground_3[i].sprite.getPosition());
    }
    for (int i = 14; i < 18; i++) {
        Vector2f vect(coloumn_3[i - 14].sprite.getGlobalBounds().width, coloumn_3[i - 14].sprite.getGlobalBounds().height);
        collision_boxs3[i].setSize(vect);
        collision_boxs3[i].setPosition(coloumn_3[i - 14].sprite.getPosition());
    }
    for (int i = 18; i < 23; i++) {
        Vector2f vect(stairs[i - 18].sprite.getGlobalBounds().width, stairs[i - 18].sprite.getGlobalBounds().height);
        collision_boxs3[i].setSize(vect);
        collision_boxs3[i].setPosition(stairs[i - 18].sprite.getPosition());
    }
    collision_boxs3[3 + 18].setScale(-1, 1);
    collision_boxs3[4 + 18].setScale(-1, 1);
    collision_boxs3[9].setSize(Vector2f(ground_3[9].sprite.getGlobalBounds().width + 190, ground_3[9].sprite.getGlobalBounds().height));
    collision_boxs3[23].setSize(Vector2f(ground_3[8].sprite.getGlobalBounds().width - 60, ground_3[8].sprite.getGlobalBounds().height + 20));
    collision_boxs3[23].setPosition(945, 180);
    
    Vector2f vect(fire_lake3.sprite.getGlobalBounds().width, fire_lake3.sprite.getGlobalBounds().height / 3);
    lakes3[0].setSize(vect);
    lakes3[0].setPosition(fire_lake3.sprite.getPosition().x, fire_lake3.sprite.getPosition().y + (fire_lake3.sprite.getGlobalBounds().height * 2 / 3));

    Vector2f vect2(green_lake_3.sprite.getGlobalBounds().width, green_lake_3.sprite.getGlobalBounds().height / 3);
    lakes3[1].setSize(vect2);
    lakes3[1].setPosition(green_lake_3.sprite.getPosition().x, green_lake_3.sprite.getPosition().y + (green_lake_3.sprite.getGlobalBounds().height * 2 / 3) - 20);
	lakes3[1].setFillColor(Color::Cyan);
    //////////////////////////////modification/////////////////////////////////////////////////
    lift_collision.setSize(Vector2f(150, 30));
    lift_collision.setPosition(23, 990 + 23);
    button_collision.setSize(Vector2f(45, 30));
    button_collision.setPosition(button.sprite.getPosition().x + 35, button.sprite.getPosition().y + 45);
    ////////////////////////////////////////////////////////////////////////////////////////////

    RenderWindow window(VideoMode(1920, 1080), "sfml works");

    View gameView(FloatRect(0, 0, 1920, 1080));
    window.setView(gameView);

    Event event;
    Clock clock;
    clock.restart();

    RectangleShape platform(Vector2f(150.0f, 30.0f));
    platform.setPosition(1450, 850);

    RectangleShape platform4(Vector2f(150.0f, 30.0f));
    platform4.setPosition(1620, 740);
    platform4.setFillColor(Color::Cyan);

    RectangleShape WALL(Vector2f(20.0f, 110.0f));
    WALL.setPosition(945 + 30, 200);
    WALL.setFillColor(Color::Red);

    character fireboy;
    fireboy.frameHeight = 140;
    Texture boytex;
    if (!boytex.loadFromFile("game_textures\\fireboy_run.png")) {
        cout << "Error: Could not load fireboy image! Check the path";
    }
    fireboy.sprite.setTexture(boytex);
    fireboy.sprite_origin();
    fireboy.framecounter = 0;
    fireboy.totalFrames = 6; //boy 6, girl 9
    fireboy.delay = 0.1f;
    fireboy.speed_x = 450.0f;
    fireboy.speed_y = 0.0f;
    fireboy.jump_strength = -950.0f;
    fireboy.gravity = 2500.0f;
    fireboy.timer = fireboy.delay;
    fireboy.onground = false;
    fireboy.ismoving = false;
    fireboy.stop = false;
    fireboy.right = Keyboard::Right;
    fireboy.left = Keyboard::Left;
    fireboy.up = Keyboard::Up;

    character watergirl;
    watergirl.frameHeight = 128;
    Texture girltex;
    if (!girltex.loadFromFile("game_textures\\watergirl_run.png")) {
        cout << "Error: Could not load watergirl image! Check the path";
    }
    watergirl.sprite.setTexture(girltex);
    watergirl.sprite_origin();
    watergirl.framecounter = 0;
    watergirl.totalFrames = 9; //boy 6, girl 9
    watergirl.delay = 0.1f;
    watergirl.speed_x = 450.0f;
    watergirl.speed_y = 0.0f;
    watergirl.jump_strength = -950.0f;
    watergirl.gravity = 2500.0f;
    watergirl.timer = watergirl.delay;
    watergirl.onground = false;
    watergirl.ismoving = false;
    watergirl.stop = false;
    watergirl.right = Keyboard::D;
    watergirl.left = Keyboard::A;
    watergirl.up = Keyboard::W;

    bool ispaused = false;

    while (window.isOpen())
    {



        float deltaTime = clock.restart().asSeconds();
        if (deltaTime > 0.1f) {
            deltaTime = 0.1f;
        }

        // time play
        if (levelTimerActive && !ispaused && (currentwindow == 2 || currentwindow == 3 || currentwindow == 4)) {
            levelTimeElapsed += deltaTime;
            // lose after 2 minutes
            if (levelTimeElapsed >= 60.f) {
                levelTimerActive = false;
                currentwindow = 8;
                gameMusic.stop();
                if (!gameOverSoundPlayed) {
                    gameOverSound.play();
                    gameOverSoundPlayed = true;
                }
            }
        }

        while (window.pollEvent(event))
        {

            switch (currentwindow)
            {
            case 0: //main menu orders

                if (Keyboard::isKeyPressed(Keyboard::Up)) MoveSelection(-1, Color::Yellow, Color::Cyan);

                if (Keyboard::isKeyPressed(Keyboard::Down)) MoveSelection(1, Color::Yellow, Color::Cyan);

                if (event.type == Event::KeyPressed && event.key.code == Keyboard::Enter)
                {

                    if (selectedmmIndex == 0) {//chosing level menu
                        currentwindow = 1;
                    }
                    else if (selectedmmIndex == 1) {//chosing credit menu
                        currentwindow = 5;
                    }
                    else if (selectedmmIndex == 2) {//chosing control menu
                        currentwindow = 6;
                    }
                    //////////////////////////////modification(2)/////////////////////////////////////////////////
                    else if (selectedmmIndex == 3) {//chosing sound menu
                        currentwindow = 10;
                    }
                    //////////////////////////////////////////////////////////////////////////////////////////
                    else if (selectedmmIndex == 4) {//chosing Exit
                        window.close();
                    }
                }




                break;

            case 1://level menu orders

                if (Keyboard::isKeyPressed(Keyboard::B)) currentwindow = 0;

                break;

            case 2://level 1

                if (Keyboard::isKeyPressed(Keyboard::Escape)) {
                    pausedlevel = currentwindow;
                    ispaused = true;
                }

                break;

            case 3://level 2

                if (Keyboard::isKeyPressed(Keyboard::Escape)) {
                    pausedlevel = currentwindow;
                    ispaused = true;
                }

                break;

            case 4://level 3

                if (Keyboard::isKeyPressed(Keyboard::Escape)) {
                    pausedlevel = currentwindow;
                    ispaused = true;
                }


                break;

            case 5://credit menu orders

                if (Keyboard::isKeyPressed(Keyboard::X)) currentwindow = 0;

                break;

            case 6://control menu orders

                if (Keyboard::isKeyPressed(Keyboard::Escape)) currentwindow = 0;

                break;

            case 7://pause menu orders

                if (Keyboard::isKeyPressed(Keyboard::Up)) MoveSelection(-1, Color::Yellow, Color::White);

                if (Keyboard::isKeyPressed(Keyboard::Down)) MoveSelection(1, Color::Yellow, Color::White);



                if (event.type == Event::KeyPressed && event.key.code == Keyboard::Enter)
                {

                    if (selectedpmIndex == 0) {//chosing resume
                        currentwindow = pausedlevel;
                        ispaused = false;
                        if (gameMusic.getStatus() != Music::Playing)
                        {
                            gameMusic.play();
                        }

                    }
                    if (selectedpmIndex == 1) {//chosing main menu
                        currentwindow = 0;
                        ispaused = false;
                        gameMusic.stop();
                        if (menuMusic.getStatus() != Music::Playing)
                        {
                            menuMusic.play();
                        }
                    }
                    if (selectedpmIndex == 2) {//chosing level menu
                        currentwindow = 1;
                        ispaused = false;
                        gameMusic.stop();
                        if (menuMusic.getStatus() != Music::Playing)
                        {
                            menuMusic.play();
                        }
                    }
                    //////////////////////////////modification(2)/////////////////////////////////////////////////
                    if (selectedpmIndex == 3) {
                        ispaused = false;
                        currentwindow = 10;
                        if (menuMusic.getStatus() != Music::Playing)
                        {
                            menuMusic.play();
                        }
                    }
                    /////////////////////////////////////////////////////////////////////////////////////////

                }

                break;
            default:
                break;

            }


            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {

                    Vector2i pixelPos(event.mouseButton.x, event.mouseButton.y);
                    Vector2f mousePos = window.mapPixelToCoords(pixelPos, gameView);

                    switch (currentwindow)
                    {
                    case 1: //level menu mouse orders
                        if (levelCards[0].getGlobalBounds().contains(mousePos)) {
                            currentwindow = 2; // Level 1
                            menuMusic.stop();
                            gameMusic.play();

                            fireboy.sprite.setColor(Color::White); watergirl.sprite.setColor(Color::White);
                            fireboy.is_dead = false; watergirl.is_dead = false;
                            smoke_framecounter = 0;
                            fireboy_score = 0;
                            watergirl_score = 0;
                            door[0].opened = false; door[0].framecounter = 0; door[0].sprite.setTextureRect(IntRect(0, 0, 121, 144)); door[0].timer = 0;
                            door[1].opened = false; door[1].framecounter = 0; door[1].sprite.setTextureRect(IntRect(0, 0, 121, 144)); door[1].timer = 0;
                            fireboy.sprite.setPosition(500, ground[2].sprite.getPosition().y - (fireboy.frameHeight / 2.0f));
                            watergirl.sprite.setPosition(400, ground[2].sprite.getPosition().y - (watergirl.frameHeight / 2.0f));
                            door[0].sprite.setPosition(50, 856 + 10 + 2);
                            door[1].sprite.setPosition(296, 1000 - 691.2 + 100 - 80 + 10 - 144 + 2);
                            for (int i = 0; i < 3; i++) { fire_point[i].sprite.setColor(Color::White); water_point[i].sprite.setColor(Color::White); }

                            points_counter = 0;
                            // start timer
                            levelTimeElapsed = 0.f;
                            levelTimerActive = true;

                        }
                        else if (levelCards[1].getGlobalBounds().contains(mousePos)) {
                            currentwindow = 3; // Level 2
                            menuMusic.stop();
                            gameMusic.play();
                            fireboy.sprite.setColor(Color::White); watergirl.sprite.setColor(Color::White);
                            fireboy.is_dead = false; watergirl.is_dead = false;
                            smoke_framecounter = 0;
                            fireboy_score = 0;
                            watergirl_score = 0;
                            door[0].opened = false; door[0].framecounter = 0; door[0].sprite.setTextureRect(IntRect(0, 0, 121, 144)); door[0].timer = 0;
                            door[1].opened = false; door[1].framecounter = 0; door[1].sprite.setTextureRect(IntRect(0, 0, 121, 144)); door[1].timer = 0;
                            fireboy.sprite.setPosition(200, ground2[5].sprite.getPosition().y - (fireboy.frameHeight / 2.0f));
                            watergirl.sprite.setPosition(100, ground2[5].sprite.getPosition().y - (watergirl.frameHeight / 2.0f));
                            door[0].sprite.setPosition(1600, 100);
                            door[1].sprite.setPosition(1750, 100);
                            for (int i = 0; i < 8; i++) { fire_point2[i].sprite.setColor(Color::White); water_point2[i].sprite.setColor(Color::White); }
                            points_counter = 0;
                            // start timer
                            levelTimeElapsed = 0.f;
                            levelTimerActive = true;
                        }
                        else if (levelCards[2].getGlobalBounds().contains(mousePos)) {
                            currentwindow = 4; // Level 3
                            menuMusic.stop();
                            gameMusic.play();
                            fireboy.sprite.setColor(Color::White); watergirl.sprite.setColor(Color::White);
                            fireboy.is_dead = false; watergirl.is_dead = false;
                            smoke_framecounter = 0;
                            fireboy_score = 0;
                            watergirl_score = 0;
                            door[0].opened = false; door[0].framecounter = 0; door[0].sprite.setTextureRect(IntRect(0, 0, 121, 144)); door[0].timer = 0;
                            door[1].opened = false; door[1].framecounter = 0; door[1].sprite.setTextureRect(IntRect(0, 0, 121, 144)); door[1].timer = 0;
                            fireboy.sprite.setPosition(1750, ground_3[2].sprite.getPosition().y - (fireboy.frameHeight / 2.0f));
                            watergirl.sprite.setPosition(150 + 120, ground_3[13].sprite.getPosition().y - (watergirl.frameHeight / 2.0f));
                            door[0].sprite.setPosition(400, 690 - 5);
                            door[1].sprite.setPosition(250, 690 - 5);
                            for (int i = 0; i < 5; i++) { fire_point3[i].sprite.setColor(Color::White); water_point3[i].sprite.setColor(Color::White); }
                            points_counter = 0;
                            // start timer
                            levelTimeElapsed = 0.f;
                            levelTimerActive = true;
                            lift.sprite.setPosition(-20, 953);
                            button.sprite.setPosition(1490, 682);
                            lift_collision.setPosition(23, 990 + 23);
                            button_collision.setPosition(button.sprite.getPosition().x + 35, button.sprite.getPosition().y + 45);
                        }
                        break;
                    case 8: // Game Over Menu
                        if (back_from_GameOver[0].getGlobalBounds().contains(mousePos)) {
                            gameOverSoundPlayed = false;
                            levelTimerActive = false;
                            levelTimeElapsed = 0.f;

                            if (menuMusic.getStatus() != Music::Playing)
                            {
                                menuMusic.play();
                            }
                            currentwindow = 0;
                            // Back to Main Menu
                        }
                        else if (back_from_GameOver[1].getGlobalBounds().contains(mousePos)) {
                            gameOverSoundPlayed = false;
                            levelTimerActive = false;
                            levelTimeElapsed = 0.f;


                            if (menuMusic.getStatus() != Music::Playing)
                            {
                                menuMusic.play();
                            }
                            currentwindow = 1; // Back to Level Menu 
                        }
                        break;
                    case 9: // Win Menu
                        //////////////////////////////modification/////////////////////////////////////////////////
                        if (btnBoxes[0].getGlobalBounds().contains(mousePos)) {
                            winSoundPlayed = false;

                            if (wonlevel == 2) {

                                currentwindow = 3;
                                menuMusic.stop();
                                gameMusic.play();
                                fireboy.sprite.setColor(Color::White); watergirl.sprite.setColor(Color::White);
                                fireboy.is_dead = false; watergirl.is_dead = false;
                                smoke_framecounter = 0;
                                fireboy_score = 0; watergirl_score = 0;
                                door[0].opened = false; door[0].framecounter = 0; door[0].sprite.setTextureRect(IntRect(0, 0, 121, 144)); door[0].timer = 0;
                                door[1].opened = false; door[1].framecounter = 0; door[1].sprite.setTextureRect(IntRect(0, 0, 121, 144)); door[1].timer = 0;
                                fireboy.sprite.setPosition(200, ground2[5].sprite.getPosition().y - (fireboy.frameHeight / 2.0f));
                                watergirl.sprite.setPosition(100, ground2[5].sprite.getPosition().y - (watergirl.frameHeight / 2.0f));
                                door[0].sprite.setPosition(1600, 100);
                                door[1].sprite.setPosition(1750, 100);
                                for (int i = 0; i < 8; i++) { fire_point2[i].sprite.setColor(Color::White); water_point2[i].sprite.setColor(Color::White); }
                                points_counter = 0;
                                levelTimeElapsed = 0.f;
                                levelTimerActive = true;
                            }
                            else if (wonlevel == 3) {

                                currentwindow = 4;
                                menuMusic.stop();
                                gameMusic.play();
                                fireboy.sprite.setColor(Color::White); watergirl.sprite.setColor(Color::White);
                                fireboy.is_dead = false; watergirl.is_dead = false;
                                smoke_framecounter = 0;
                                fireboy_score = 0; watergirl_score = 0;
                                door[0].opened = false; door[0].framecounter = 0; door[0].sprite.setTextureRect(IntRect(0, 0, 121, 144)); door[0].timer = 0;
                                door[1].opened = false; door[1].framecounter = 0; door[1].sprite.setTextureRect(IntRect(0, 0, 121, 144)); door[1].timer = 0;
                                fireboy.sprite.setPosition(1750, ground_3[2].sprite.getPosition().y - (fireboy.frameHeight / 2.0f));
                                watergirl.sprite.setPosition(150 + 120, ground_3[13].sprite.getPosition().y - (watergirl.frameHeight / 2.0f));
                                door[0].sprite.setPosition(400, 690 - 5);
                                door[1].sprite.setPosition(250, 690 - 5);
                                for (int i = 0; i < 5; i++) { fire_point3[i].sprite.setColor(Color::White); water_point3[i].sprite.setColor(Color::White); }


                                button.sprite.setPosition(1490, 682);
                                button_collision.setPosition(button.sprite.getPosition().x + 35, button.sprite.getPosition().y + 45);
                                lift.sprite.setPosition(-20, 953);
                                lift_collision.setPosition(23, 990 + 23);

                                points_counter = 0;
                                levelTimeElapsed = 0.f;
                                levelTimerActive = true;
                            }
                            else {
                                if (menuMusic.getStatus() != Music::Playing)
                                {
                                    menuMusic.play();
                                }
                                currentwindow = 5;
                            }
                        }
                        else if (btnBoxes[1].getGlobalBounds().contains(mousePos)) {
                            winSoundPlayed = false;

                            if (menuMusic.getStatus() != Music::Playing)
                            {
                                menuMusic.play();
                            }
                            currentwindow = 1; // Back to Level Menu 
                        }
                        /////////////////////////////////////////////////////////////////////////////////////////////////
                        break;

                        //////////////////////////////modification(2)/////////////////////////////////////////////////
                    case 10:


                        if (sound_btnBoxes[0].getGlobalBounds().contains(mousePos)) {
                            isMenuMusicOn = !isMenuMusicOn;
                            if (isMenuMusicOn) {
                                sound_txt[0].setString("Main Menu Music : ON");
                                sound_txt[0].setFillColor(Color(80, 255, 100));
                                if (menuMusic.getStatus() != Music::Playing) menuMusic.play();
                            }
                            else {
                                sound_txt[0].setString("Main Menu Music : OFF");
                                sound_txt[0].setFillColor(Color(255, 80, 80));
                                menuMusic.pause();
                            }
                            sound_txt[0].setOrigin(sound_txt[0].getLocalBounds().width / 2.0f, sound_txt[0].getLocalBounds().height / 2.0f);
                            sound_txt[0].setPosition(1920 / 2.0f, 400 - 15); //the new positin
                        }
                        else if (sound_btnBoxes[1].getGlobalBounds().contains(mousePos)) {
                            isGameMusicOn = !isGameMusicOn;
                            if (isGameMusicOn) {
                                sound_txt[1].setString("Levels Music : ON");
                                sound_txt[1].setFillColor(Color(80, 255, 100));
                                gameMusic.setVolume(35);
                            }
                            else {
                                sound_txt[1].setString("Levels Music : OFF");
                                sound_txt[1].setFillColor(Color(255, 80, 80));
                                gameMusic.setVolume(0);
                            }
                            sound_txt[1].setOrigin(sound_txt[1].getLocalBounds().width / 2.0f, sound_txt[1].getLocalBounds().height / 2.0f);
                            sound_txt[1].setPosition(1920 / 2.0f, 580 - 15); //the new positin
                        }
                        else if (sound_btnBoxes[2].getGlobalBounds().contains(mousePos)) {
                            currentwindow = 0;
                        }

                        break;
                        ////////////////////////////////////////////////////////////////////////////////////////////////////////

                    default:
                        break;
                    }
                }
            }


            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (!ispaused) {
            fireboy.onground = false;
            watergirl.onground = false;
            fireboy.stop = false;
            watergirl.stop = false;

            switch (currentwindow)
            {
            case 2:   //level 1 collisions
                for (int i = 3; i < 12; i++) {
                    platform_collision(fireboy, collision_boxs[i]);
                    platform_collision(watergirl, collision_boxs[i]);
                }
                platform_collision(fireboy, walls[3]);
                platform_collision(watergirl, walls[3]);
                //platform_collision(fireboy, walls[2]);
                //platform_collision(watergirl, walls[2]);
                platform_collision(fireboy, platform);
                platform_collision(watergirl, platform);

                for (int i = 1; i <= 2; i++) {
                    platform_collision(fireboy, collision_boxs[i]);
                    platform_collision(watergirl, collision_boxs[i]);
                }
                for (int i = 0; i < 2; i++) {
                    lakes_collision(watergirl, fireboy, lakes[i], smoke, deltaTime, deathSound, gameMusic);
                    lakes_collision(fireboy, watergirl, lakes[i + 2], smoke, deltaTime, deathSound, gameMusic);
                }
                lakes_collision(fireboy, watergirl, lakes[4], smoke, deltaTime, deathSound, gameMusic);
                for (int i = 0; i < 2; i++) {
                    wall_collision(fireboy, watergirl, walls[i]);
                    wall_collision(fireboy, watergirl, walls[i]);
                }
                wall_collision(fireboy, watergirl, collision_boxs[0]);
                wall_collision(fireboy, watergirl, collision_boxs[0]);

                platform_collision(fireboy, collision_boxs[2]);
                platform_collision(watergirl, collision_boxs[2]);
                platform_collision(fireboy, collision_boxs[1]);
                platform_collision(watergirl, collision_boxs[1]);

                slope_collision(fireboy, collision_boxs[3].getPosition().x - 64, collision_boxs[3].getPosition().y + 64, collision_boxs[3].getPosition().x, collision_boxs[3].getPosition().y);
                slope_collision(fireboy, collision_boxs[8].getPosition().x - 64, collision_boxs[8].getPosition().y + 64, collision_boxs[8].getPosition().x, collision_boxs[8].getPosition().y);
                slope_collision(fireboy, collision_boxs[9].getPosition().x - 64, collision_boxs[9].getPosition().y + 64, collision_boxs[9].getPosition().x, collision_boxs[9].getPosition().y);
                slope_collision(fireboy, 627, 655, 692, 559);
                slope_collision(fireboy, 1505, 470, 1645, 580); //triangle
                slope_collision(fireboy, 1365, 580, 1505, 470); //tringle
                slope_collision(fireboy, 700, 987, 764, 1045); // fire lake 2
                slope_collision(fireboy, 1165, 1045, 1218, 987); //fire lake 2
                slope_collision(fireboy, 1258, 655, 1274, 617); //fire lake 3
                slope_collision(fireboy, 1038, 559, 1104, 657); //fire lake 3
                slope_collision(fireboy, 456, 618, 475, 656); //water lake 0
                slope_collision(fireboy, 1502, 227, 1575, 289); //ground 11
                slope_collision(fireboy, 1275, 270, 1295, 230); //fire lake 4
                slope_collision(fireboy, 1090, 230, 1111, 270); //fire lake 4
                slope_collision(fireboy, 730, 230, 750, 270); //water lake 1
                slope_collision(fireboy, 913, 270, 933, 230); //water lake 1

                slope_collision(watergirl, collision_boxs[3].getPosition().x - 64, collision_boxs[3].getPosition().y + 64, collision_boxs[3].getPosition().x + 10, collision_boxs[3].getPosition().y - 10);
                slope_collision(watergirl, collision_boxs[8].getPosition().x - 64, collision_boxs[8].getPosition().y + 64, collision_boxs[8].getPosition().x, collision_boxs[8].getPosition().y);
                slope_collision(watergirl, collision_boxs[9].getPosition().x - 64, collision_boxs[9].getPosition().y + 64, collision_boxs[9].getPosition().x, collision_boxs[9].getPosition().y);
                slope_collision(watergirl, 627, 655, 692, 559);
                slope_collision(watergirl, 1505, 480, 1645, 570); //triangle
                slope_collision(watergirl, 1365, 570, 1505, 480); //tringle
                slope_collision(watergirl, 700, 987, 764, 1045); // fire lake 2
                slope_collision(watergirl, 1165, 1045, 1218, 987); //fire lake 2
                slope_collision(watergirl, 1258, 655, 1274, 617); //fire lake 3
                slope_collision(watergirl, 1038, 559, 1104, 657); //fire lake 3
                slope_collision(watergirl, 456, 618, 475, 656); //water lake 0
                slope_collision(watergirl, 1502, 227, 1575, 289); //ground 11
                slope_collision(watergirl, 1275, 270, 1295, 230); //fire lake 4
                slope_collision(watergirl, 1090, 230, 1110, 270); //fire lake 4
                slope_collision(watergirl, 730, 230, 750, 270); //water lake 1
                slope_collision(watergirl, 913, 270, 933, 230); //water lake 1

                door_collision(fireboy, door[0], deltaTime);
                door_collision(watergirl, door[1], deltaTime);

                if (door[0].opened && door[1].opened) {
                    bool is_win = 0;
                    static float timer = 0;
                    timer += deltaTime;
                    if (timer >= 0.16f) {
                        timer = 0.0f;
                        if ((door[0].framecounter >= 6 && door[0].framecounter < 17) && (door[1].framecounter >= 6 && door[1].framecounter < 17)) {
                            door[0].framecounter++;
                            door[0].sprite.setTextureRect(IntRect(door[0].framecounter * 121, 0, 121, 144));
                            door[1].framecounter++;
                            door[1].sprite.setTextureRect(IntRect(door[1].framecounter * 121, 0, 121, 144));
                            watergirl.sprite.setColor(Color::Transparent);
                            watergirl.speed_x = 0;
                            watergirl.speed_y = 0;
                            fireboy.sprite.setColor(Color::Transparent);
                            fireboy.speed_x = 0;
                            fireboy.speed_y = 0;
                            if (door[0].framecounter >= 17 || door[1].framecounter >= 17) {
                                door[0].sprite.setTextureRect(IntRect(17 * 121, 0, 121, 144));
                                door[1].sprite.setTextureRect(IntRect(17 * 121, 0, 121, 144));
                                is_win = true;
                            }
                        }
                        if (is_win)
                        {
                            wonlevel = 2;
                            currentwindow = 9;
                            // best time & score
                            if (fireboy_score > highScore_fire[0]) highScore_fire[0] = fireboy_score;
                            if (watergirl_score > highScore_water[0]) highScore_water[0] = watergirl_score;
                            if (levelTimeElapsed < bestTime[0]) bestTime[0] = levelTimeElapsed;

                            menuMusic.stop();
                            gameMusic.stop();

                            if (!winSoundPlayed)
                            {
                                gameMusic.stop();
                                winSound.play();
                                winSoundPlayed = true;
                            }
                        }
                    }
                }
                else {
                    if (door[0].opened && door[0].framecounter >= 6 && !door[1].opened) {
                        door[0].framecounter = 6;
                        door[0].sprite.setTextureRect(IntRect(door[0].framecounter * 121, 0, 121, 144));
                    }
                    if (door[1].opened && door[1].framecounter >= 6 && !door[0].opened) {
                        door[1].framecounter = 6;
                        door[1].sprite.setTextureRect(IntRect(door[1].framecounter * 121, 0, 121, 144));
                    }
                }


                for (int i = 0; i < 3; i++)
                    point_collision(fireboy, watergirl, fire_point[i], fireboy_score, collectSound, wrongCollectSound);
                for (int i = 0; i < 3; i++)
                    point_collision(watergirl, fireboy, water_point[i], watergirl_score, collectSound, wrongCollectSound);

                for (int i = 0; i < 135; i++) {
                    platform_collision(fireboy, rec[i]);
                    platform_collision(watergirl, rec[i]);
                }
                for (int i = 0; i < 3; i++)
                    lake_animation(fire[i], deltaTime);
                for (int i = 0; i < 2; i++)
                    lake_animation(water[i], deltaTime);

                moveCharacter(fireboy, deltaTime);
                moveCharacter(watergirl, deltaTime);
                jumpCharacter(fireboy, deltaTime);
                jumpCharacter(watergirl, deltaTime);
                animation(fireboy, deltaTime);
                animation(watergirl, deltaTime);

                if ((fireboy.is_dead && smoke_framecounter >= 5) || (watergirl.is_dead && smoke_framecounter >= 5 && currentwindow != 8)) {
                    currentwindow = 8;
                    if (!gameOverSoundPlayed)
                    {
                        gameOverSound.play();
                        gameOverSoundPlayed = true;
                    }
                    cout << "lol";
                }

                break;
            case 3: //level 2 collisions

                for (int i = 0; i < 16; i++) {  //16
                    if (i >= 5 && i <= 7) {
                        platform_collision(fireboy, collision_boxs2[i]);
                        platform_collision(watergirl, collision_boxs2[i]);
                    }
                    else {
                        platform_collision(fireboy, collision_boxs2[i]);
                        platform_collision(watergirl, collision_boxs2[i]);
                    }
                }
                wall_collision(fireboy, watergirl, collision_boxs2[16]);
                wall_collision(fireboy, watergirl, collision_boxs2[17]);
                wall_collision(fireboy, watergirl, collision_boxs2[2]);
                wall_collision(fireboy, watergirl, collision_boxs2[15]);


                for (int i = 1; i < 8; i++)
                    point_collision(fireboy, watergirl, fire_point2[i], fireboy_score, collectSound, wrongCollectSound);
                for (int i = 1; i < 8; i++)
                    point_collision(watergirl, fireboy, water_point2[i], watergirl_score, collectSound, wrongCollectSound);

                for (int i = 0; i < 2; i++) {
                    lakes_collision(watergirl, fireboy, lakes2[i], smoke, deltaTime, deathSound, gameMusic);
                    acid_collision(fireboy, watergirl, lakes2[i + 2], smoke, deltaTime, deathSound, gameMusic);
                    lakes_collision(fireboy, watergirl, lakes2[i + 4], smoke, deltaTime, deathSound, gameMusic);
                }

                platform_collision(watergirl, platform4);
                platform_collision(fireboy, platform4);

                collision_boxs2[19].setFillColor(Color::Red);

                // right_corner2
                wall_collision(fireboy, watergirl, collision_boxs2[13]); //13
                wall_collision(fireboy, watergirl, collision_boxs2[13]); //13

                // step_corner2 
                wall_collision(fireboy, watergirl, collision_boxs2[14]);
                wall_collision(fireboy, watergirl, collision_boxs2[14]);

                slope_collision(fireboy, 380, 1016, 410, 1056); //left fire
                slope_collision(fireboy, 625, 1056, 655, 1016); //left fire
                slope_collision(fireboy, 1186, 1016, 1216, 1056); //right water
                slope_collision(fireboy, 1431, 1056, 1461, 1016); //right water
                slope_collision(fireboy, 315, 830, 355, 860); //left water
                slope_collision(fireboy, 670, 860, 730, 830); //left water
                slope_collision(fireboy, 1136, 830, 1186, 860); //right fire
                slope_collision(fireboy, 1471, 860, 1511, 830); //right fire
                slope_collision(fireboy, 265, 600, 305, 640); //left acid
                slope_collision(fireboy, 625, 640, 665, 600); //left acid
                slope_collision(fireboy, 942, 600, 992, 640); //right acid
                slope_collision(fireboy, 1302, 640, 1352, 600); //right acid
                slope_collision(fireboy, 280, 300, 400, 235); //slope near right corner

                slope_collision(watergirl, 380, 1016, 410, 1056); //left fire
                slope_collision(watergirl, 625, 1056, 655, 1016); //left fire
                slope_collision(watergirl, 1186, 1016, 1216, 1056); //right water
                slope_collision(watergirl, 1431, 1056, 1461, 1016); //right water
                slope_collision(watergirl, 335, 830, 375, 860); //left water
                slope_collision(watergirl, 650, 860, 710, 830); //left water
                slope_collision(watergirl, 1136, 830, 1186, 860); //right fire
                slope_collision(watergirl, 1471, 860, 1511, 830); //right fire
                slope_collision(watergirl, 265, 600, 305, 640); //left acid
                slope_collision(watergirl, 625, 640, 665, 600); //left acid
                slope_collision(watergirl, 942, 600, 992, 640); //right acid
                slope_collision(watergirl, 1302, 640, 1352, 600); //right acid
                slope_collision(watergirl, 275, 300, 395, 235); //slope near right corner

                door_collision(fireboy, door[0], deltaTime);
                door_collision(watergirl, door[1], deltaTime);

                if (door[0].opened && door[1].opened) {
                    bool is_win = false;
                    static float timer = 0;
                    timer += deltaTime;
                    if (timer >= 0.16f) {
                        timer = 0.0f;
                        if ((door[0].framecounter >= 6 && door[0].framecounter < 17) && (door[1].framecounter >= 6 && door[1].framecounter < 17)) {
                            door[0].framecounter++;
                            door[0].sprite.setTextureRect(IntRect(door[0].framecounter * 121, 0, 121, 144));
                            door[1].framecounter++;
                            door[1].sprite.setTextureRect(IntRect(door[1].framecounter * 121, 0, 121, 144));
                            watergirl.sprite.setColor(Color::Transparent);
                            watergirl.speed_x = 0;
                            watergirl.speed_y = 0;
                            fireboy.sprite.setColor(Color::Transparent);
                            fireboy.speed_x = 0;
                            fireboy.speed_y = 0;
                            if (door[0].framecounter >= 17 || door[1].framecounter >= 17) {
                                door[0].sprite.setTextureRect(IntRect(16 * 121, 0, 121, 144));
                                door[1].sprite.setTextureRect(IntRect(16 * 121, 0, 121, 144));
                                is_win = true;

                            }

                        }
                        if (is_win)
                        {
                            wonlevel = 3;
                            currentwindow = 9;

                            // best time & score
                            if (fireboy_score > highScore_fire[1]) highScore_fire[1] = fireboy_score;
                            if (watergirl_score > highScore_water[1]) highScore_water[1] = watergirl_score;
                            if (levelTimeElapsed < bestTime[1]) bestTime[1] = levelTimeElapsed;

                            menuMusic.stop();
                            gameMusic.stop();

                            if (!winSoundPlayed)
                            {
                                winSound.play();
                                winSoundPlayed = true;
                            }
                        }
                    }
                }
                else {
                    if (door[0].opened && door[0].framecounter >= 6 && !door[1].opened) {
                        door[0].framecounter = 6;
                        door[0].sprite.setTextureRect(IntRect(door[0].framecounter * 121, 0, 121, 144));
                    }
                    if (door[1].opened && door[1].framecounter >= 6 && !door[0].opened) {
                        door[1].framecounter = 6;
                        door[1].sprite.setTextureRect(IntRect(door[1].framecounter * 121, 0, 121, 144));
                    }
                }
                for (int i = 0; i < 2; i++) {
                    lake_animation(acid2[i], deltaTime);
                    lake_animation(fire2[i], deltaTime);
                    lake_animation(water2[i], deltaTime);
                }
                moveCharacter(fireboy, deltaTime);
                moveCharacter(watergirl, deltaTime);
                jumpCharacter(fireboy, deltaTime);
                jumpCharacter(watergirl, deltaTime);
                animation(fireboy, deltaTime);
                animation(watergirl, deltaTime);

                if ((fireboy.is_dead && smoke_framecounter >= 5) || (watergirl.is_dead && smoke_framecounter >= 5)) {
                    currentwindow = 8;
                    if (!gameOverSoundPlayed)
                    {
                        gameOverSound.play();
                        gameOverSoundPlayed = true;
                    }
                }

                break;

            case 4: //level 3 collisions
                //////////////////////////////modification/////////////////////////////////////////////////
                for (int i = 0; i < 24; i++) {
                    if (i == 18 || i == 19 || i == 20 || i == 21 || i == 22) {

                    }
                    else {
                        platform_collision(fireboy, collision_boxs3[i]);
                        platform_collision(watergirl, collision_boxs3[i]);
                    }
                }
                //////////////////////////////////////////////////////////////////////////////////////////////
                lakes_collision(fireboy, watergirl, lakes3[0], smoke, deltaTime, deathSound, gameMusic);
                acid_collision(fireboy, watergirl, lakes3[1], smoke, deltaTime, deathSound, gameMusic);


                for (int i = 14; i < 18; i++) {
                    wall_collision(fireboy, watergirl, collision_boxs3[i]);
                }

                for (int i = 0; i < 2; i++) {
                    wall_collision(fireboy, watergirl, walls[i]);
                    wall_collision(fireboy, watergirl, walls[i]);
                }
                wall_collision(fireboy, watergirl, WALL);
                slope_collision(watergirl, 845, 306, 945, 177);
                slope_collision(fireboy, 845, 306, 945, 177);

                snow_slope_collision(fireboy, watergirl, 1550 + 40 + 35, 1080 - 15, 1550 + 380, 900, deltaTime);  //snow slope


                ///////////////////modification/////////////////////////////////////////////////
                platform_collision(watergirl, lift_collision);
                platform_collision(fireboy, lift_collision);
                platform_collision(watergirl, button_collision);
                platform_collision(fireboy, button_collision);

                slope_collision(watergirl, 1525 - 18, 727 + 23, 1525, 727);
                slope_collision(fireboy, 1525 - 18, 727 + 23, 1525, 727);
                slope_collision(watergirl, 1525 + 45, 727, 1525 + 45 + 18, 727 + 23);
                slope_collision(fireboy, 1525 + 45, 727, 1525 + 45 + 18, 727 + 23);

                FloatRect hitbox1 = watergirl.sprite.getGlobalBounds();
                hitbox1.width = 40;
                hitbox1.left += 65;
                hitbox1.height += 5.0f;
                FloatRect hitbox2 = fireboy.sprite.getGlobalBounds();
                hitbox2.width = 40;
                hitbox2.left += 65;
                hitbox2.height += 5.0f;
                FloatRect lift_bounds = lift_collision.getGlobalBounds();
                lift_bounds.top = lift_bounds.top + lift_bounds.height;
                lift_bounds.height = 5;

                if (button_collision.getGlobalBounds().intersects(hitbox2) || button_collision.getGlobalBounds().intersects(hitbox1)) {
                    if (lift_collision.getPosition().y > 300) {
                        if (lift_collision.getGlobalBounds().intersects(hitbox1)) {
                            watergirl.sprite.move(0, -300 * deltaTime);
                        }
                        if (lift_collision.getGlobalBounds().intersects(hitbox2)) {
                            fireboy.sprite.move(0, -300 * deltaTime);
                        }
                        lift_collision.move(0, -300 * deltaTime);
                        lift.sprite.move(0, -300 * deltaTime);
                        if (button.sprite.getPosition().y < 695) {
                            button.sprite.move(0, 20 * deltaTime);
                            button_collision.move(0, 20 * deltaTime);
                        }
                    }
                }
                else {
                    bool isBlocked = false;
                    if (lift_bounds.intersects(hitbox1))
                        isBlocked = true;
                    else
                        isBlocked = false;
                    if (lift_collision.getPosition().y < 1013 && !isBlocked) {
                        lift_collision.move(0, 300 * deltaTime);
                        lift.sprite.move(0, 300 * deltaTime);
                    }
                    if (button.sprite.getPosition().y > 682) {
                        button.sprite.move(0, -20 * deltaTime);
                        button_collision.move(0, -20 * deltaTime);
                    }
                }
                /////////////////////////////////////////////////////////////////////////////////////////
                door_collision(fireboy, door[0], deltaTime);
                door_collision(watergirl, door[1], deltaTime);
                if (door[0].opened && door[1].opened) {
                    bool is_win = false;
                    static float timer = 0;
                    timer += deltaTime;
                    if (timer >= 0.16f) {
                        timer = 0.0f;
                        if ((door[0].framecounter >= 6 && door[0].framecounter < 17) && (door[1].framecounter >= 6 && door[1].framecounter < 17)) {
                            door[0].framecounter++;
                            door[0].sprite.setTextureRect(IntRect(door[0].framecounter * 121, 0, 121, 144));
                            door[1].framecounter++;
                            door[1].sprite.setTextureRect(IntRect(door[1].framecounter * 121, 0, 121, 144));
                            watergirl.sprite.setColor(Color::Transparent);
                            watergirl.speed_x = 0;
                            watergirl.speed_y = 0;
                            fireboy.sprite.setColor(Color::Transparent);
                            fireboy.speed_x = 0;
                            fireboy.speed_y = 0;
                            if (door[0].framecounter >= 17 || door[1].framecounter >= 17) {
                                door[0].sprite.setTextureRect(IntRect(16 * 121, 0, 121, 144));
                                door[1].sprite.setTextureRect(IntRect(16 * 121, 0, 121, 144));
                                is_win = true;
                            }
                        }
                        if (is_win)
                        {
                            wonlevel = 4;
                            currentwindow = 9;

                            // best time & score
                            if (fireboy_score > highScore_fire[2]) highScore_fire[2] = fireboy_score;
                            if (watergirl_score > highScore_water[2]) highScore_water[2] = watergirl_score;
                            if (levelTimeElapsed < bestTime[2]) bestTime[2] = levelTimeElapsed;

                            menuMusic.stop();
                            gameMusic.stop();

                            if (!winSoundPlayed)
                            {
                                winSound.play();
                                winSoundPlayed = true;
                            }
                        }
                    }
                }
                else {
                    if (door[0].opened && door[0].framecounter >= 6 && !door[1].opened) {
                        door[0].framecounter = 6;
                        door[0].sprite.setTextureRect(IntRect(door[0].framecounter * 121, 0, 121, 144));
                    }
                    if (door[1].opened && door[1].framecounter >= 6 && !door[0].opened) {
                        door[1].framecounter = 6;
                        door[1].sprite.setTextureRect(IntRect(door[1].framecounter * 121, 0, 121, 144));
                    }
                }
                for (int i = 0; i < 5; i++) {
                    point_collision(fireboy, watergirl, fire_point3[i], fireboy_score, collectSound, wrongCollectSound);
                    point_collision(watergirl, fireboy, water_point3[i], watergirl_score, collectSound, wrongCollectSound);
                }

                lake_animation(fire3, deltaTime);
                lake_animation(acid3, deltaTime);

                moveCharacter(fireboy, deltaTime);
                moveCharacter(watergirl, deltaTime);
                jumpCharacter(fireboy, deltaTime);
                jumpCharacter(watergirl, deltaTime);
                animation(fireboy, deltaTime);
                animation(watergirl, deltaTime);

                if ((fireboy.is_dead && smoke_framecounter >= 5) || (watergirl.is_dead && smoke_framecounter >= 5)) {
                    currentwindow = 8;

                    if (!gameOverSoundPlayed)
                    {
                        gameOverSound.play();
                        gameOverSoundPlayed = true;
                    }
                }
                break;
            }

        }
        else {
            currentwindow = 7;
            gameMusic.pause();
        }




        window.clear();

        window.setView(gameView);

        int timeLeft;

        switch (currentwindow)
        {
        case 0://Drawing the main menu


            window.draw(menu_BackGground);
            for (int i = 0; i < 5; i++)
            {
                window.draw(main_txt[i]);
            }

            break;

        case 1://Drawing level menu 


            window.draw(levelsMenuBG);
            window.draw(levelsTitle);

            for (int i = 0; i < 3; i++) {
                window.draw(levelCards[i]);
                window.draw(levelTexts[i]);
            }
            window.draw(levelsBackText);


            for (int i = 0; i < 3; i++) {
                // high score
                string fireHS = "Fire Best: " + to_string(highScore_fire[i]);
                string waterHS = "Water Best: " + to_string(highScore_water[i]);
                highScoreTxt[i].setString(fireHS + "\n" + waterHS);
                window.draw(highScoreTxt[i]);

                // best time
                if (bestTime[i] < 999.f) {
                    int secs = (int)bestTime[i];
                    bestTimeTxt[i].setString("Best Time: " + to_string(secs) + "s");
                }
                else {
                    bestTimeTxt[i].setString("Best Time: --");
                }
                window.draw(bestTimeTxt[i]);
            }

            break;

        case 2://drawin level 1

            for (int i = 0; i < 12; i++)
                window.draw(collision_boxs[i]);
            for (int i = 0; i < 4; i++)
                window.draw(walls[i]);
            for (int i = 0; i < 5; i++)
                window.draw(lakes[i]);

            window.draw(background.sprite);
            window.draw(Frame.sprite);

            window.draw(door[0].sprite);
            window.draw(door[1].sprite);


            if (!fireboy.is_dead)
                window.draw(fireboy.sprite);
            else {
                window.draw(smoke.sprite);
            }

            if (!watergirl.is_dead)
                window.draw(watergirl.sprite);
            else {
                window.draw(smoke.sprite);
            }


            for (int i = 0; i < 12; i++)
                window.draw(ground[i].sprite);

            window.draw(Platform[0].sprite);
            window.draw(Platform[1].sprite);

            for (int i = 0; i < 3; i++) {
                window.draw(fire[i].sprite);
            }

            for (int i = 0; i < 2; i++) {
                window.draw(water[i].sprite);
            }

            window.draw(ground[5].sprite);
            window.draw(triangle.sprite);


            for (int i = 0; i < 3; i++)
                window.draw(fire_point[i].sprite);

            for (int i = 0; i < 3; i++)
                window.draw(water_point[i].sprite);

            // draw score

            scoreTxt[0].setString("Fire: " + to_string(fireboy_score));
            scoreTxt[1].setString("Water: " + to_string(watergirl_score));
            window.draw(scoreTxt[0]);
            window.draw(scoreTxt[1]);

            // draw timer

            timeLeft = 60 - (int)levelTimeElapsed;
            if (timeLeft < 0) timeLeft = 0;
            timerTxt.setFillColor(timeLeft <= 20 ? Color::Red : Color::White);
            timerTxt.setString("Time: " + to_string(timeLeft));
            window.draw(timerTxt);



            break;

        case 3://drawin level 2

            window.draw(background2.sprite);
            for (int i = 0; i < 10; i++) {
                if (!(i == 2)) {
                    window.draw(ground2[i].sprite);
                }
            }
            for (int i = 0; i < 3; i++)
                window.draw(platform2[i].sprite);

            window.draw(right_corner2.sprite);
            window.draw(step_corner2.sprite);
            window.draw(slope_right2.sprite);


            for (int i = 1; i < 8; i++)
                window.draw(fire_point2[i].sprite);

            for (int i = 1; i < 8; i++)
                window.draw(water_point2[i].sprite);

            window.draw(Frame.sprite);

            window.draw(door[0].sprite);
            window.draw(door[1].sprite);

            if (!fireboy.is_dead)
                window.draw(fireboy.sprite);
            else {
                window.draw(smoke.sprite);
            }

            if (!watergirl.is_dead)
                window.draw(watergirl.sprite);
            else {
                window.draw(smoke.sprite);
            }

            window.draw(acid2[0].sprite);
            window.draw(acid2[1].sprite);
            window.draw(fire2[0].sprite);
            window.draw(fire2[1].sprite);
            window.draw(water2[0].sprite);
            window.draw(water2[1].sprite);

            // draw score

            scoreTxt[0].setString("Fire: " + to_string(fireboy_score));
            scoreTxt[1].setString("Water: " + to_string(watergirl_score));
            window.draw(scoreTxt[0]);
            window.draw(scoreTxt[1]);

            // draw timer

            timeLeft = 60 - (int)levelTimeElapsed;
            if (timeLeft < 0) timeLeft = 0;
            timerTxt.setFillColor(timeLeft <= 20 ? Color::Red : Color::White);
            timerTxt.setString("Time: " + to_string(timeLeft));
            window.draw(timerTxt);

            break;

        case 4://drawin lvel 3

            window.draw(background.sprite);


            for (int i = 0; i < 14; i++) {
                window.draw(ground_3[i].sprite);
            }

            for (int i = 0; i < 4; i++) {
                window.draw(coloumn_3[i].sprite);
            }

            window.draw(corner_3.sprite);

            for (int i = 0; i < 5; i++)
                window.draw(fire_point3[i].sprite);

            for (int i = 0; i < 5; i++)
                window.draw(water_point3[i].sprite);

            //    for (int i = 0; i < 5; i++)
              //      window.draw(stairs[i].sprite);

            window.draw(Frame.sprite);

            window.draw(door[0].sprite);
            window.draw(door[1].sprite);
            //////////////////////////////modification/////////////////////////////////////////////////
            window.draw(button.sprite);
            window.draw(ground_3[3].sprite);
            window.draw(lift.sprite);
            ////////////////////////////////////////////////////////////////////////////////////////////
            if (!fireboy.is_dead)
                window.draw(fireboy.sprite);
            else {
                window.draw(smoke.sprite);
            }

            if (!watergirl.is_dead)
                window.draw(watergirl.sprite);
            else {
                window.draw(smoke.sprite);
            }

            window.draw(fire3.sprite);
            window.draw(acid3.sprite);
            window.draw(SNOW_3.sprite);
            window.draw(WALL);
            // draw score

            scoreTxt[0].setString("Fire: " + to_string(fireboy_score));
            scoreTxt[1].setString("Water: " + to_string(watergirl_score));
            window.draw(scoreTxt[0]);
            window.draw(scoreTxt[1]);

            // draw timer

            timeLeft = 60 - (int)levelTimeElapsed;
            if (timeLeft < 0) timeLeft = 0;
            timerTxt.setFillColor(timeLeft <= 20 ? Color::Red : Color::White);
            timerTxt.setString("Time: " + to_string(timeLeft));
            window.draw(timerTxt);

            break;

        case 5://drawin credit menu

            window.draw(credit_BG);
            for (int i = 0; i < 8; i++) {

                window.draw(namesarr[i]);
            }


            break;

        case 6://drawin control menu

            window.draw(c_BG);
            window.draw(F_BG);
            window.draw(W_BG);

            for (int i = 0; i < 2; i++) {
                window.draw(control_txt[i]);
            }

            break;

        case 7://drawin pause menu

            window.draw(menu_BG);
            for (int i = 0; i < 4; i++) {
                window.draw(pause_txt[i]);
            }

            break;

        case 8://drawin game over widow

            window.draw(gameOverText);
            for (int i = 0; i < 2; i++)
            {
                window.draw(back_from_GameOver[i]);
            }

            break;

        case 9://drawin win widnow

            window.draw(winMenuBG);
            window.draw(winTitleShadow);
            window.draw(winTitle);

            for (int i = 0; i < 2; i++) {
                window.draw(btnBoxes[i]);
                window.draw(winOptions[i]);
            }

            // draw score

            scoreTxt[0].setString("Fire Score: " + to_string(fireboy_score));
            scoreTxt[1].setString("Water Score: " + to_string(watergirl_score));
            scoreTxt[0].setPosition(750, 400);
            scoreTxt[1].setPosition(750, 500);
            window.draw(scoreTxt[0]);
            window.draw(scoreTxt[1]);

            // position score
            scoreTxt[0].setPosition(50, 20);
            scoreTxt[1].setPosition(300, 20);

            break;
            //////////////////////////////modification(2)/////////////////////////////////////////////////
        case 10:
            window.draw(levelsMenuBG);
            window.draw(sound_title);
            for (int i = 0; i < 3; i++) {
                window.draw(sound_btnBoxes[i]);
                window.draw(sound_txt[i]);
            }
            break;
            /////////////////////////////////////////////////////////////////////////////////////////////

        default:
            break;
        }

        window.display();


    }
    return 0;
}