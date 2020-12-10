/*
----------------------------

Bullsquid

----------------------------
*/

$modelname npc/bullsquid_water.mdl

$cdmaterials models\npc\bullsquid_water

$body studio "Bullsquid_reference.smd" scale 2.5

$eyeposition 0 0 24

$sequence wateridle "Water_idle" loop fps 30 scale 2.5
$sequence waterswim "Water_swim" loop fps 30 scale 2.5
$sequence wateremerge "Water_emerge" fps 30 scale 2.5
$sequence watersubmerge "Water_submerge" fps 30 scale 2.5
$sequence waterattack "Water_attack" fps 30 scale 2.5


$include "hitbox.qci"
$include "ragdoll.qci"

//==============================================================================
/*
$lod 10 
{
	replacemodel "Bullsquid_reference" "Bullsquid_LOD_1"
}

$lod 20
{
	replacemodel "Bullsquid_reference" "Bullsquid_LOD_2"
	bonetreecollapse "Bullsquid.tail3"
	replacebone "Bullsquid.eye_l" "Bullsquid.head"
	replacebone "Bullsquid.eye_r" "Bullsquid.head"
	replacebone "Bullsquid.neck" "Bullsquid.head"
}

$lod 40
{
	replacemodel "Bullsquid_reference" "Bullsquid_LOD_3"
	bonetreecollapse "Bullsquid.tail2"
	replacebone "Bullsquid.eye_l" "Bullsquid.head"
	replacebone "Bullsquid.eye_r" "Bullsquid.head"
	bonetreecollapse "Bullsquid.carpus_r"
	bonetreecollapse "Bullsquid.carpus_L"
	replacebone "Bullsquid.neck" "Bullsquid.head"
}

$lod 60 
{
	replacemodel "Bullsquid_reference" "Bullsquid_LOD_4"
	bonetreecollapse "Bullsquid.tail1"
	replacebone "Bullsquid.eye_l" "Bullsquid.head"
	replacebone "Bullsquid.eye_r" "Bullsquid.head"
	bonetreecollapse "Bullsquid.carpus_r"
	bonetreecollapse "Bullsquid.carpus_L"
	replacebone "Bullsquid.tentacle_top_r2" "Bullsquid.tentacle_top_r1"
	replacebone "Bullsquid.tentacle_top_r3" "Bullsquid.tentacle_top_r1"
	replacebone "Bullsquid.tentacle_top_l2" "Bullsquid.tentacle_top_l1"
	replacebone "Bullsquid.tentacle_top_l3" "Bullsquid.tentacle_top_l1"
	replacebone "Bullsquid.tentacle_bottom_r2" "Bullsquid.tentacle_bottom_r1"
	replacebone "Bullsquid.tentacle_bottom_r3" "Bullsquid.tentacle_bottom_r1"
	replacebone "Bullsquid.tentacle_bottom_l2" "Bullsquid.tentacle_bottom_l1"
	replacebone "Bullsquid.tentacle_bottom_l3" "Bullsquid.tentacle_bottom_l1"
	replacebone "Bullsquid.neck" "Bullsquid.head"
}

$shadowlod
{
	replacemodel "Bullsquid_reference" "Bullsquid_LOD_shadow"
	replacebone "Bullsquid.eye_l" "Bullsquid.head"
	replacebone "Bullsquid.eye_r" "Bullsquid.head"
}
*/
//==============================================================================