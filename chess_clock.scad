/****************************************************************************
Chess Clock
by Matt Braddock (slashinfty)
https://github.com/slashinfty/chess-clock

MIT License

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
*****************************************************************************/

/*****************
parameters to edit
*****************/

// these determine overall dimensions
// but are not the actual dimensions
length = 160;
button_height = 70;
button_width = 40;
front_ledge = 15;

// these determine hole sizes
button_diameter = 29.5;
encoder_diameter = 7;
screen_x = 30.5;
screen_y = 14.5;
switch_x = 12.7;
switch_y = 6.9;

// additional parameters
wall = 3;
roundness = 2.5;

/*******************
calculated constants
*******************/

slant_xy = button_height - front_ledge;
full_width = slant_xy + button_width;
hole_height = wall + roundness + 1;

/************
miscellaneous
************/

$fn = $preview ? 15 : 75;

/*****************
constructed shapes
*****************/

module shell() {
    translate([roundness, roundness, roundness])
    minkowski() {
        sphere(r = roundness);
        polyhedron(points = [
            [0, 0, 0],
            [length, 0, 0],
            [0, 0, front_ledge],
            [length, 0, front_ledge],
            [0, slant_xy, button_height],
            [length, slant_xy, button_height],
            [0, full_width, button_height],
            [length, full_width, button_height],
            [0, full_width, 0],
            [length, full_width, 0]
        ], faces = [
            [0, 2, 3, 1],
            [2, 4, 5, 3],
            [4, 6, 7, 5],
            [6, 8, 9, 7],
            [8, 0, 1, 9],
            [1, 3, 5, 7, 9],
            [8, 6, 4, 2, 0]
        ]);
    }
}

module interior() {
    diff = 2 * wall - 2 * roundness;
    translate([wall, wall, 0])
    polyhedron(points = [
        [0, 0, 0],
        [length - diff, 0, 0],
        [0, 0, front_ledge - diff],
        [length - diff, 0, front_ledge - diff],
        [0, slant_xy - diff, button_height - diff],
        [length - diff, slant_xy - diff, button_height - diff],
        [0, full_width - diff, button_height - diff],
        [length - diff, full_width - diff, button_height - diff],
        [0, full_width - diff, 0],
        [length - diff, full_width - diff, 0]
    ], faces = [
        [0, 2, 3, 1],
        [2, 4, 5, 3],
        [4, 6, 7, 5],
        [6, 8, 9, 7],
        [8, 0, 1, 9],
        [1, 3, 5, 7, 9],
        [8, 6, 4, 2, 0]
    ]);
}

module topHoles() {
    translate([0, 0, 0])
    union() {
        cylinder(d = button_diameter, h = hole_height);
        translate([0, 0, 0])
        cylinder(d = button_diameter, h = hole_height);
    }
}

module slantHoles() {
    translate([0, 0, 0])
    rotate([45, 0, 0])
    union() {
        cube([screen_x, screen_y, hole_height]);
        translate([0, 0, 0])
        cube([screen_x, screen_y, hole_height]);
        translate([0, 0, 0])
        cylinder(d = encoder_diameter, h = hole_height);
    }
}

module switchHole() {
    translate([0, 0, 0])
    cube([hole_height, switch_x, switch_y]);
}

/***********
final output
***********/

//translate([0, full_width + roundness * 2, button_height + roundness * 2])rotate([180, 0, 0])
difference() {
    shell();
    interior();
    //topHoles();
    //slantHoles();
    //switchHole();
}