#include "TileSet.h"

// With permission, this table was directly converted from BahKooJ's MeshData.txt file to this file.

using Data::Mission::Til::Mesh::BACK_LEFT;
using Data::Mission::Til::Mesh::BACK_RIGHT;
using Data::Mission::Til::Mesh::FRONT_RIGHT;
using Data::Mission::Til::Mesh::FRONT_LEFT;


using Data::Mission::Til::Mesh::RED;
using Data::Mission::Til::Mesh::GREEN;
using Data::Mission::Til::Mesh::BLUE;
using Data::Mission::Til::Mesh::NO_ELEMENT;

const unsigned Data::Mission::Til::TileSet::POLYGON_COUNT = 128;

Data::Mission::Til::Mesh::Polygon Data::Mission::Til::TileSet::default_mesh[POLYGON_COUNT] = {
    { // 0
        {
             {BACK_LEFT, RED},
             {FRONT_RIGHT, RED},
             {BACK_RIGHT, RED},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, RED},
             {FRONT_RIGHT, RED},
             {BACK_LEFT, RED},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {BACK_LEFT, GREEN},
             {FRONT_RIGHT, GREEN},
             {BACK_RIGHT, GREEN},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, GREEN},
             {FRONT_RIGHT, GREEN},
             {BACK_LEFT, GREEN},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {BACK_LEFT, BLUE},
             {FRONT_RIGHT, BLUE},
             {BACK_RIGHT, BLUE},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    { // 5
        {
             {FRONT_LEFT, BLUE},
             {FRONT_RIGHT, BLUE},
             {BACK_LEFT, BLUE},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, RED},
             {FRONT_RIGHT, RED},
             {BACK_RIGHT, RED},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, RED},
             {BACK_RIGHT, RED},
             {BACK_LEFT, RED},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, GREEN},
             {FRONT_RIGHT, GREEN},
             {BACK_RIGHT, GREEN},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, GREEN},
             {BACK_RIGHT, GREEN},
             {BACK_LEFT, GREEN},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    { // 10
        {
             {FRONT_LEFT, BLUE},
             {FRONT_RIGHT, BLUE},
             {BACK_RIGHT, BLUE},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, BLUE},
             {BACK_RIGHT, BLUE},
             {BACK_LEFT, BLUE},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, GREEN},
             {FRONT_RIGHT, RED},
             {BACK_RIGHT, RED},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, GREEN},
             {BACK_RIGHT, RED},
             {BACK_LEFT, GREEN},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, BLUE},
             {FRONT_RIGHT, GREEN},
             {BACK_RIGHT, GREEN},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    { // 15
        {
             {FRONT_LEFT, BLUE},
             {BACK_RIGHT, GREEN},
             {BACK_LEFT, BLUE},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {BACK_LEFT, GREEN},
             {FRONT_RIGHT, RED},
             {BACK_RIGHT, RED},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, GREEN},
             {FRONT_RIGHT, RED},
             {BACK_LEFT, GREEN},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {BACK_LEFT, BLUE},
             {FRONT_RIGHT, GREEN},
             {BACK_RIGHT, GREEN},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, BLUE},
             {FRONT_RIGHT, GREEN},
             {BACK_LEFT, BLUE},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    { // 20
        {
             {FRONT_LEFT, RED},
             {BACK_RIGHT, GREEN},
             {BACK_LEFT, RED},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, RED},
             {FRONT_RIGHT, GREEN},
             {BACK_RIGHT, GREEN},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, GREEN},
             {BACK_RIGHT, BLUE},
             {BACK_LEFT, GREEN},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, GREEN},
             {FRONT_RIGHT, BLUE},
             {BACK_RIGHT, BLUE},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, RED},
             {FRONT_RIGHT, GREEN},
             {BACK_LEFT, RED},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    { // 25
        {
             {BACK_LEFT, RED},
             {FRONT_RIGHT, GREEN},
             {BACK_RIGHT, GREEN},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, GREEN},
             {FRONT_RIGHT, BLUE},
             {BACK_LEFT, GREEN},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {BACK_LEFT, GREEN},
             {FRONT_RIGHT, BLUE},
             {BACK_RIGHT, BLUE},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, RED},
             {FRONT_RIGHT, RED},
             {BACK_RIGHT, GREEN},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, RED},
             {BACK_RIGHT, GREEN},
             {BACK_LEFT, GREEN},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    { // 30
        {
             {FRONT_LEFT, GREEN},
             {FRONT_RIGHT, GREEN},
             {BACK_RIGHT, BLUE},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, GREEN},
             {BACK_RIGHT, BLUE},
             {BACK_LEFT, BLUE},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, RED},
             {FRONT_RIGHT, RED},
             {BACK_LEFT, GREEN},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {BACK_LEFT, GREEN},
             {FRONT_RIGHT, RED},
             {BACK_RIGHT, GREEN},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, GREEN},
             {FRONT_RIGHT, GREEN},
             {BACK_LEFT, BLUE},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    { // 35
        {
             {BACK_LEFT, BLUE},
             {FRONT_RIGHT, GREEN},
             {BACK_RIGHT, BLUE},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {BACK_LEFT, RED},
             {FRONT_RIGHT, GREEN},
             {BACK_RIGHT, RED},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, GREEN},
             {FRONT_RIGHT, GREEN},
             {BACK_LEFT, RED},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {BACK_LEFT, GREEN},
             {FRONT_RIGHT, BLUE},
             {BACK_RIGHT, GREEN},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, BLUE},
             {FRONT_RIGHT, BLUE},
             {BACK_LEFT, GREEN},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    { // 40
        {
             {FRONT_LEFT, GREEN},
             {BACK_RIGHT, RED},
             {BACK_LEFT, RED},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, GREEN},
             {FRONT_RIGHT, GREEN},
             {BACK_RIGHT, RED},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, BLUE},
             {BACK_RIGHT, GREEN},
             {BACK_LEFT, GREEN},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, BLUE},
             {FRONT_RIGHT, BLUE},
             {BACK_RIGHT, GREEN},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {BACK_LEFT, GREEN},
             {FRONT_RIGHT, GREEN},
             {BACK_RIGHT, RED},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    { // 45
        {
             {FRONT_LEFT, RED},
             {FRONT_RIGHT, GREEN},
             {BACK_LEFT, GREEN},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {BACK_LEFT, BLUE},
             {FRONT_RIGHT, BLUE},
             {BACK_RIGHT, GREEN},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, GREEN},
             {FRONT_RIGHT, BLUE},
             {BACK_LEFT, BLUE},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, GREEN},
             {FRONT_RIGHT, RED},
             {BACK_RIGHT, GREEN},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, GREEN},
             {BACK_RIGHT, GREEN},
             {BACK_LEFT, RED},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    { // 50
        {
             {FRONT_LEFT, BLUE},
             {FRONT_RIGHT, GREEN},
             {BACK_RIGHT, BLUE},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, BLUE},
             {BACK_RIGHT, BLUE},
             {BACK_LEFT, GREEN},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, GREEN},
             {FRONT_RIGHT, RED},
             {BACK_LEFT, RED},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {BACK_LEFT, RED},
             {FRONT_RIGHT, RED},
             {BACK_RIGHT, GREEN},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, BLUE},
             {FRONT_RIGHT, GREEN},
             {BACK_LEFT, GREEN},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    { // 55
        {
             {BACK_LEFT, GREEN},
             {FRONT_RIGHT, GREEN},
             {BACK_RIGHT, BLUE},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, RED},
             {FRONT_RIGHT, GREEN},
             {BACK_RIGHT, RED},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, RED},
             {BACK_RIGHT, RED},
             {BACK_LEFT, GREEN},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, GREEN},
             {FRONT_RIGHT, BLUE},
             {BACK_RIGHT, GREEN},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, GREEN},
             {BACK_RIGHT, GREEN},
             {BACK_LEFT, BLUE},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    { // 60
        {
             {FRONT_LEFT, GREEN},
             {BACK_LEFT, GREEN},
             {FRONT_RIGHT, RED},
             {BACK_RIGHT, RED}
        },
        false
    },
    {
        {
             {FRONT_LEFT, BLUE},
             {BACK_LEFT, BLUE},
             {FRONT_RIGHT, GREEN},
             {BACK_RIGHT, GREEN}
        },
        false
    },
    {
        {
             {FRONT_LEFT, RED},
             {BACK_LEFT, RED},
             {FRONT_RIGHT, GREEN},
             {BACK_RIGHT, GREEN}
        },
        false
    },
    {
        {
             {FRONT_LEFT, GREEN},
             {BACK_LEFT, GREEN},
             {FRONT_RIGHT, BLUE},
             {BACK_RIGHT, BLUE}
        },
        false
    },
    {
        {
             {FRONT_LEFT, RED},
             {BACK_LEFT, GREEN},
             {FRONT_RIGHT, RED},
             {BACK_RIGHT, GREEN}
        },
        false
    },
    { // 65
        {
             {FRONT_LEFT, GREEN},
             {BACK_LEFT, BLUE},
             {FRONT_RIGHT, GREEN},
             {BACK_RIGHT, BLUE}
        },
        false
    },
    {
        {
             {FRONT_LEFT, GREEN},
             {BACK_LEFT, RED},
             {FRONT_RIGHT, GREEN},
             {BACK_RIGHT, RED}
        },
        false
    },
    {
        {
             {FRONT_LEFT, BLUE},
             {BACK_LEFT, GREEN},
             {FRONT_RIGHT, BLUE},
             {BACK_RIGHT, GREEN}
        },
        false
    },
    {
        {
             {FRONT_LEFT, RED},
             {BACK_LEFT, RED},
             {FRONT_RIGHT, RED},
             {BACK_RIGHT, RED}
        },
        false
    },
    {
        {
             {FRONT_LEFT, GREEN},
             {BACK_LEFT, GREEN},
             {FRONT_RIGHT, GREEN},
             {BACK_RIGHT, GREEN}
        },
        false
    },
    { // 70
        {
             {FRONT_LEFT, BLUE},
             {BACK_LEFT, BLUE},
             {FRONT_RIGHT, BLUE},
             {BACK_RIGHT, BLUE}
        },
        false
    },
    {
        {
             {FRONT_LEFT, RED},
             {FRONT_RIGHT, RED},
             {FRONT_LEFT, GREEN},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, GREEN},
             {FRONT_RIGHT, RED},
             {FRONT_RIGHT, GREEN},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, GREEN},
             {FRONT_RIGHT, GREEN},
             {FRONT_LEFT, BLUE},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, BLUE},
             {FRONT_RIGHT, GREEN},
             {FRONT_RIGHT, BLUE},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    { // 75
        {
             {FRONT_LEFT, RED},
             {FRONT_RIGHT, RED},
             {FRONT_RIGHT, GREEN},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, RED},
             {FRONT_RIGHT, GREEN},
             {FRONT_LEFT, GREEN},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, GREEN},
             {FRONT_RIGHT, GREEN},
             {FRONT_RIGHT, BLUE},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, GREEN},
             {FRONT_RIGHT, BLUE},
             {FRONT_LEFT, BLUE},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, RED},
             {BACK_LEFT, RED},
             {BACK_LEFT, GREEN},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    { // 80
        {
             {FRONT_LEFT, RED},
             {BACK_LEFT, GREEN},
             {FRONT_LEFT, GREEN},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, GREEN},
             {BACK_LEFT, GREEN},
             {BACK_LEFT, BLUE},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, GREEN},
             {BACK_LEFT, BLUE},
             {FRONT_LEFT, BLUE},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, RED},
             {BACK_LEFT, RED},
             {FRONT_LEFT, GREEN},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, GREEN},
             {BACK_LEFT, RED},
             {BACK_LEFT, GREEN},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    { // 85
        {
             {FRONT_LEFT, GREEN},
             {BACK_LEFT, GREEN},
             {FRONT_LEFT, BLUE},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, BLUE},
             {BACK_LEFT, GREEN},
             {BACK_LEFT, BLUE},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_RIGHT, RED},
             {BACK_LEFT, GREEN},
             {BACK_LEFT, RED},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_RIGHT, RED},
             {FRONT_RIGHT, GREEN},
             {BACK_LEFT, GREEN},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_RIGHT, GREEN},
             {BACK_LEFT, BLUE},
             {BACK_LEFT, GREEN},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    { // 90
        {
             {FRONT_RIGHT, GREEN},
             {FRONT_RIGHT, BLUE},
             {BACK_LEFT, BLUE},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_RIGHT, RED},
             {FRONT_RIGHT, GREEN},
             {BACK_LEFT, RED},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_RIGHT, GREEN},
             {BACK_LEFT, GREEN},
             {BACK_LEFT, RED},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_RIGHT, GREEN},
             {FRONT_RIGHT, BLUE},
             {BACK_LEFT, GREEN},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_RIGHT, BLUE},
             {BACK_LEFT, BLUE},
             {BACK_LEFT, GREEN},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    { // 95
        {
             {FRONT_LEFT, RED},
             {BACK_RIGHT, RED},
             {BACK_RIGHT, GREEN},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, RED},
             {BACK_RIGHT, GREEN},
             {FRONT_LEFT, GREEN},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, GREEN},
             {BACK_RIGHT, GREEN},
             {BACK_RIGHT, BLUE},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, GREEN},
             {BACK_RIGHT, BLUE},
             {FRONT_LEFT, BLUE},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, RED},
             {BACK_RIGHT, RED},
             {FRONT_LEFT, GREEN},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    { // 100
        {
             {FRONT_LEFT, GREEN},
             {BACK_RIGHT, RED},
             {BACK_RIGHT, GREEN},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, GREEN},
             {BACK_RIGHT, GREEN},
             {FRONT_LEFT, BLUE},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_LEFT, BLUE},
             {BACK_RIGHT, GREEN},
             {BACK_RIGHT, BLUE},
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {FRONT_RIGHT, RED},
             {FRONT_RIGHT, GREEN},
             {BACK_LEFT, RED},
             {BACK_LEFT, GREEN}
        },
        false
    },
    {
        {
             {FRONT_RIGHT, GREEN},
             {FRONT_RIGHT, BLUE},
             {BACK_LEFT, GREEN},
             {BACK_LEFT, BLUE}
        },
        false
    },
    { // 105
        {
             {FRONT_LEFT, RED},
             {FRONT_LEFT, GREEN},
             {BACK_RIGHT, RED},
             {BACK_RIGHT, GREEN}
        },
        false
    },
    {
        {
             {FRONT_LEFT, GREEN},
             {FRONT_LEFT, BLUE},
             {BACK_RIGHT, GREEN},
             {BACK_RIGHT, BLUE}
        },
        false
    },
    {
        {
             {FRONT_LEFT, RED},
             {FRONT_LEFT, GREEN},
             {FRONT_RIGHT, RED},
             {FRONT_RIGHT, GREEN}
        },
        false
    },
    {
        {
             {FRONT_LEFT, RED},
             {FRONT_LEFT, GREEN},
             {BACK_LEFT, RED},
             {BACK_LEFT, GREEN}
        },
        false
    },
    {
        {
             {FRONT_LEFT, GREEN},
             {FRONT_LEFT, BLUE},
             {FRONT_RIGHT, GREEN},
             {FRONT_RIGHT, BLUE}
        },
        false
    },
    { // 110
        {
             {FRONT_LEFT, GREEN},
             {FRONT_LEFT, BLUE},
             {BACK_LEFT, GREEN},
             {BACK_LEFT, BLUE}
        },
        false
    },
    {
        {
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    { // 115
        {
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    { // 120
        {
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    { // 125
        {
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },
    {
        {
             {BACK_LEFT, NO_ELEMENT}
        },
        false
    },

};
