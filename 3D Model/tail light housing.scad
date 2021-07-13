use <DCDC_Converter.scad>;
use <ESP8266Models.scad>;

want_lens = 0;
want_ring_top = 0;
want_print_ready_rotate = 0;

z_height_adj = 2;

$fn=100;

wall_thickness = 2;
water_tight_overhang_y = 2;

watertight_y_offset = 9 + 6; //extra clearance

neopixel_outerRadius = 45 / 2;
neopixel_innerRadius = 31.37 / 2 - 0.2;
neopixel_height = 3.26;

outer_dim_x = 60;
outer_dim_y = 45 + wall_thickness*2;
outer_dim_z = 38 + z_height_adj;

inner_dim_x = 58;
inner_dim_y = 45;
inner_dim_z = outer_dim_z - neopixel_height;

wire_bend_y = 16; //dcdc wire channel
dcdc_outer_dim_z = 22;

sphere_z = 50;
sphere_d = outer_dim_x - 2;

diffuser_lens_diameter = sphere_d-5;
diffuser_lens_thickness = 0.4;

switch_z_offset = 29;

cable_diameter = 3.5 + 0.2;

/* ***************************** */

if (want_print_ready_rotate)    {rotate([-180,0,0]) main_body();} else {main_body();} 

/* ***************************** */

module main_body()
{
    difference()
    {
        union()
        {
            outer_box();
        }
        
        union()
        {
            inner_cavity();
            translate([0,0,-0.01]) dcdc_converter();
            translate([0,wire_bend_y/2,diffuser_lens_thickness+sphere_z/2]) neopixel_ring();
            /* for alignment only */ //translate([0,0,26]) WemosD1M(pins=0);
        }
    }
    water_tight_edge();

    //transparent lid.
    if (want_lens) color("red") translate([0,wire_bend_y/2,sphere_z]) cylinder(h=diffuser_lens_thickness, d=diffuser_lens_diameter);

    screw_mounts();
}
/* ***************************** */

screw_holder_width = 5.8;
holder_diameter = 16;
holder_outreach_z = 21;

module screw_mount(x_position, y_offset)
{
    screw_hole_diameter = 4.05;
    
    
    difference()
    {
        union()
        {
            translate([x_position,y_offset,-holder_outreach_z]) rotate([0,90,0])
            cylinder(d=holder_diameter, h=screw_holder_width);
            
            translate([x_position,y_offset-holder_diameter/2,-holder_outreach_z])
            cube([screw_holder_width,holder_diameter,holder_outreach_z]);
        }
    
    
        translate([x_position-0.1,y_offset,-holder_outreach_z]) rotate([0,90,0])
        cylinder(d=screw_hole_diameter, h=screw_holder_width+0.2);
    }
}

module draw_nut()
{
    nut_outer_size = 6.82 + 0.2;
    nut_edge_size = 4 + 0.02;
    nut_thickness = 2.94 + 1;
    nut_sides = 6;
    
    translate([nut_thickness,0,0])
    for ( i = [0 : nut_sides] ){
        rotate([i*(360/nut_sides),0,0]) cube([nut_edge_size,nut_thickness,nut_outer_size], center=true);
    }
}

module screw_mounts()
{
    holder_total_width_outside = 27.4;
    holder_total_width_inside = holder_total_width_outside - screw_holder_width*2;
    strength_block_z = 7.13;
    
    y_offset = outer_dim_y/2+wire_bend_y/2-wall_thickness;
    
    //screw holders
    //nut side
    nut_side_x_pos = holder_total_width_inside/2;
    difference()
    {
        screw_mount(nut_side_x_pos, y_offset);
        translate([nut_side_x_pos-0.1,y_offset,-holder_outreach_z]) draw_nut();
        
    }
    
    //screw side
    screw_side_x_pos = -holder_total_width_inside/2-screw_holder_width;
    screw_head_clearance_diameter = 11.3;
    difference()
    {
        screw_mount(screw_side_x_pos, y_offset);
        translate([screw_side_x_pos-0.1,y_offset,-holder_outreach_z]) rotate([0,90,0]) cylinder(d=screw_head_clearance_diameter, h=3.94);
    }
    
    //strength block
    translate([0,y_offset,-strength_block_z/2])
    cube([holder_total_width_outside,holder_diameter,strength_block_z], center=true);
}

module water_tight_edge()
{
    new_y = outer_dim_y - inner_dim_y;
    
    //top
    translate([0,outer_dim_y/2-new_y/2+watertight_y_offset/2,wall_thickness/2]) cube([outer_dim_x,outer_dim_y/2+wire_bend_y/2-watertight_y_offset,wall_thickness], center=true);
    
    //bottom
    translate([0,-outer_dim_y/2+new_y/2,wall_thickness/2]) cube([outer_dim_x,new_y,wall_thickness], center=true);
}


module outer_box()
{
    outer_x = outer_dim_x;
    outer_y = outer_dim_y;
    outer_z = outer_dim_z;
    
    //outer edges
    translate([0,wire_bend_y/2,outer_z/2]) cube([outer_x,outer_y+wire_bend_y,outer_z], center=true); 
    
    //rounded top
    if (want_ring_top) intersection()
    {
        translate([0,wire_bend_y/2,outer_z])sphere(d=sphere_d);
        translate([0,wire_bend_y/2,sphere_z/2]) cube([outer_x,outer_y+wire_bend_y,sphere_z], center=true); 
    }
    
    
    draw_switch_outline();
}

module inner_cavity()
{
    inner_wire_bend_y = wire_bend_y - wall_thickness;
    
    //cube cutout
    translate([-inner_dim_x/2,-inner_dim_y/2,-0.01]) cube([inner_dim_x, inner_dim_y + inner_wire_bend_y, inner_dim_z]);
    
    //ring wall thickness
    translate([0,wire_bend_y/2,diffuser_lens_thickness+sphere_z/2]) translate([0,0,-13]) cylinder(r=neopixel_innerRadius + 0.1,h=outer_dim_z-wall_thickness);
    
    draw_switch_holes();
    draw_cable_hole();
}

module neopixel_ring()
{
    $fn = 120;

    centerRadius = (neopixel_outerRadius + neopixel_innerRadius) / 2;
    ringThickness = (neopixel_outerRadius - neopixel_innerRadius);

    difference() 
    {
        cylinder(r=neopixel_outerRadius,h=outer_dim_z);
        translate([0,0,-13])
        cylinder(r=neopixel_innerRadius,h=outer_dim_z+0.02);
    }

}

module draw_cable_hole()
{
    cutout_height = wall_thickness+wire_bend_y;
    
    translate([0,-outer_dim_y/2+cutout_height-0.1,outer_dim_z/2]) rotate([90,0,0]) cylinder(d=cable_diameter,h=cutout_height);
}

/* ***************************** */

switch_hole_diameter = 12;
shell_thickness = wall_thickness;
switch_outline_pad = 2.25;
ring_stick_out = 0.5;

hole_depth = shell_thickness + ring_stick_out;

module draw_switch_cyl(diameter, height, offset)
{
    //switch_hole_diameter
    translate([offset,0,switch_z_offset]) rotate([90,0,90]) cylinder(h=height, d=diameter);
}

module draw_switch_outline()
{
    draw_switch_cyl(switch_hole_diameter+switch_outline_pad, ring_stick_out, outer_dim_x/2);
}

module draw_switch_holes()
{
    translate([0.01,0,0]) draw_switch_cyl(switch_hole_diameter, ring_stick_out+shell_thickness, outer_dim_x/2-shell_thickness);
    
    translate([-1,0,0]) draw_switch_cyl(switch_hole_diameter+7, 10, outer_dim_x/2-10);
}
