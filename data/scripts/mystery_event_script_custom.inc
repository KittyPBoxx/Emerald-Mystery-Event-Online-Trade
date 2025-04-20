@ This code runs from ramscript. Because of ASLR the location it runs from in ram will not be constant
@ This means calls between methods in this file must use relative addresses. 
@ setvaddress sets a relative address then you use macros such as vmessage, vgoto, vcall e.t.c
@ Obviously if you can still make constant calls to areas in rom, or areas in ram where you've copied something
@ If you're calling a special that would cause the memory layout to change (like ChoosePartyMon) you need to move that into the constant section

.section .mystery_event_script
MysteryEvent_EventScript_CustomEvent::
    setvaddress MysteryEvent_EventScript_CustomEvent
	lock
	faceplayer
	vmessage MysteryEvent_Text_Explaination_Text
	waitbuttonpress
	msgbox CableClub_Text_TradePokemon, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, EventScript_CancelSmash
	compare_ptr_to_value 0x02024218, 1
	vgoto_if FALSE, MysteryEvent_EventScript_Offline
	goto MysteryEvent_EventScript_PickMon
	end

MysteryEvent_EventScript_Offline::
	vmessage MysteryEvent_Text_Offline_Text
	waitbuttonpress
	release
	end

MysteryEvent_Text_Offline_Text::
	.string "You appear to be offline.\n"
	.string "Reset the game for a new connection.$"

MysteryEvent_Text_Explaination_Text::
	.string "Here you can trade your POKéMON\n"
	.string "over the internet!$"

.previous

@ If you want to test this inside the rom you need to switch the sections
@ .section .rom_function

@ VAR_0x8004 [Picked Mon from player party] [In game trade names]
@ VAR_0x8005 [Party index for ingame trade] [Party index for online trade]
@ VAR_0x8003 [0=No Trade partner, 1=Trade partner Found, 2=Retry Trade]

.section .ram_function
MysteryEvent_EventScript_PickMon::
	special ChoosePartyMon
	waitstate
	setvar VAR_0x8003, 0
	goto_if_eq VAR_0x8004, PARTY_NOTHING_CHOSEN, EventScript_CancelSmash
	copyvar VAR_0x8005, VAR_0x8004
	specialvar VAR_RESULT, ScriptGetPartyMonSpecies
	bufferspeciesname STR_VAR_1, VAR_RESULT
	msgbox MysteryEvent_EventScript_ConfirmTrade_Text, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, EventScript_CancelSmash
	goto MysteryEvent_EventScript_Trade
	end

MysteryEvent_EventScript_Trade::
	callnative myEvent
	waitstate
	goto_if_eq VAR_0x8003, 0, MysteryEvent_EventScript_TradeNoPartner
	setvar VAR_0x8004, 2
	special DoInGameTradeScene
	waitstate
	release
	end

MysteryEvent_EventScript_TradeNoPartner::
	setvar VAR_0x8003, 2
	msgbox MysteryEvent_EventScript_NoPartner_Text, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, EventScript_CancelSmash
	goto MysteryEvent_EventScript_Trade
	end

MysteryEvent_EventScript_NoPartner_Text::
	.string "No partner was found...\n"
	.string "Do you want to retry?$"

MysteryEvent_EventScript_ConfirmTrade_Text::
	.string "So you want to trade {STR_VAR_1}?$"

.previous
