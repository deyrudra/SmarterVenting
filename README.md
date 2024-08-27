# SmarterVenting
This is a Smarter Venting system that uses a hierarchy for prioritization of when to open vents, and at which degree.

*Note*: This is the main directory, which uses the esp8266 d1 mini microcontroller. The esp32 directory, using the LolinD32 (esp32) microcontroller.

Hierarchy:
1. Priority Rooms
   - Rooms above a garage.
   - Rooms with lack of insulation
   - Rooms with heating/cooling issues due to the size.
2. Secondary Rooms
   - Rooms that still need heating/cooling, but do not have any issues stated above.
3. Thermostat Location
  - Depending on the location, it could be an array of vents, or a singular vent.

Hierarchy concept:
- The general concept is as simple as reordering the rooms in which they get cool/hot air through.
- To prioritize the location of the thermostat last is to let the HVAC system run without stopping up until the Thermostat location reaches it's desired temperature.
  - This allows the other rooms to cooled/heated to it's desired temperature first.
 
Fallback:
- The temperature of the air coming from the vent; that is the limit to which you can cool/heat your room.
