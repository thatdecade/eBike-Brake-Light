
wall_thickness = 2;

outer_dim_x = 58;
outer_dim_y = 40.1;
outer_dim_z = 22;

screwhole_offset_from_center = 65/2;
screwhole_diameter = 3.7;
screw_holder_radius = 4.64;
screw_holder_z = 4;
screw_holder_angle = 35;

module dcdc_converter()
{
    translate([-outer_dim_x/2,-outer_dim_y/2,0]) cube([outer_dim_x, outer_dim_y, outer_dim_z]);

    //screw_holder(screwhole_offset_from_center);
    translate([-screwhole_offset_from_center,0,0]) screw_holder();
    translate([screwhole_offset_from_center,0,0]) rotate([0,0,180]) screw_holder();
    
    wire_channel();
}

module wire_channel()
{
    wire_bend_x = outer_dim_x;
    wire_bend_y = 10;
    wire_bend_z = outer_dim_z - wall_thickness;
    wire_bend_z_offset = outer_dim_z-wire_bend_z;
    wire_bend_z_overhang = 10;
    
    //allow for wire bend radius
    translate([0,outer_dim_y/2+wire_bend_y/2,wire_bend_z_offset+(wire_bend_z+wire_bend_z_overhang)/2]) cube([wire_bend_x, wire_bend_y, wire_bend_z + wire_bend_z_overhang], center=true);
}

module screw_holder()
{
    difference()
    {
        union()
        {
            //flange
            translate([0,0,0]) cylinder(h=screw_holder_z, r=screw_holder_radius, $fn=25);
             
            //angled flange edge
            translate([0,0,0]) 
            rotate([0,0,screw_holder_angle]) cube([screw_holder_radius*2,screw_holder_radius,screw_holder_z]);
            translate([0,0,0]) rotate([0,0,-screw_holder_angle]) translate([0,-screw_holder_radius,0]) cube([screw_holder_radius*2,screw_holder_radius,screw_holder_z]);
        }
        
        //hole
        translate([0,0,-0.1]) cylinder(h=screw_holder_z+0.2, d=screwhole_diameter, $fn=25);
    }
}

dcdc_converter();