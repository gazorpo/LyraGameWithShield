# Lyra with Shield

The Lyra project is to provide a bootstrapping game for Unreal Engine.  Lyra is intended to be a living sample that shows how we build scalable games around the engines core technology.

This repo adds a shield to the health so damage is done to the shield first and only excess damage is done to the health. Extra work would be required to show the shield status on the screen. If you want to make the shield recharge, you can use blueprints like in the images here: https://forums.unrealengine.com/t/how-to-auto-recharge-health-shield-after-delay-in-lyra/546606/2?u=gazorpo

See here for the changes made to the base Lyra 5.1 code: https://github.com/gazorpo/LyraGameWithShield/commit/9e013985c441c4f67721d1bceb1f8d2cf189caff

Or see here for the 5.0 version: https://github.com/gazorpo/LyraGameWithShield/commit/646d74c854eb513a36201b6e50348664fe4c5317

## How to add shield to Lyra shooter UI

In Unreal Engine, open the Content Drawer or Content Browser, and go to Content > UI > Hud > W_Healthbar.

<img width="485" alt="" src="https://user-images.githubusercontent.com/89604529/211082112-196032e4-2d6d-46e8-afdb-e59a0bc045aa.png">

Right-click it, click Duplicate, and name the new version W_Shieldbar. Then double-click it and go to the Event Graph as shown below. (If you do not see it immediately, click the Graph button in the top-right part of the window.)

<img width="1154" alt="" src="https://user-images.githubusercontent.com/89604529/211082213-01c249b2-0fe7-4682-9c50-97addf3349c7.png">

Now you have to replace the three health nodes above with shield ones. You can find them by dragging off the blue dots and searching for the shield versions.

<img width="455" alt="" src="https://user-images.githubusercontent.com/89604529/211082290-b9a88a7b-bf00-4f4d-a4f6-e8b877ed17bf.png">

Make sure they are connected up like the old health ones. Then go to Plugins > ShooterCore Content > UserInterface > W_ShooterHUDLayout and double-click it.

<img width="653" alt="" src="https://user-images.githubusercontent.com/89604529/211082321-0af64f14-53da-4ee9-8b33-ea9975677510.png">

Finally, search W_Shieldbar in the Palette and drag it onto the HUD layout.

<img width="1019" alt="" src="https://user-images.githubusercontent.com/89604529/211082393-28af6394-e0dc-4083-ad58-e2104f3c0ebe.png">
