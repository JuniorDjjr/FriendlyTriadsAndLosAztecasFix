#include "plugin.h"
#include "CTheScripts.h"
#include "extensions/ScriptCommands.h"
#include "..\injector\assembly.hpp"

using namespace plugin;
using namespace injector;

bool friendlyTriads = false;
bool lastFriendlyTriads = false;
bool friendlyLosAztecas = false;
bool lastFriendlyLosAztecas = false;

class FriendlyTriadsAndLosAztecasFix
{
public:
    FriendlyTriadsAndLosAztecasFix()
	{

		Events::processScriptsEvent += []
		{
			int WUZIMU_TOTAL_PASSED_MISSIONS = *(int*)(CTheScripts::ScriptSpace + (543 * 4));
			int RIOT_TOTAL_PASSED_MISSIONS = *(int*)(CTheScripts::ScriptSpace + (629 * 4));

			if (WUZIMU_TOTAL_PASSED_MISSIONS > 0)
			{
				if (!lastFriendlyTriads)
				{
					friendlyTriads = true;

					Command<COMMAND_SET_RELATIONSHIP>(1, ePedType::PED_TYPE_GANG2, ePedType::PED_TYPE_GANG7);
					Command<COMMAND_SET_RELATIONSHIP>(1, ePedType::PED_TYPE_GANG7, ePedType::PED_TYPE_GANG2);
					if (friendlyLosAztecas)
					{
						Command<COMMAND_SET_RELATIONSHIP>(1, ePedType::PED_TYPE_GANG7, ePedType::PED_TYPE_GANG8);
						Command<COMMAND_SET_RELATIONSHIP>(1, ePedType::PED_TYPE_GANG8, ePedType::PED_TYPE_GANG7);
					}

					lastFriendlyTriads = friendlyTriads;
				}
			}
			else
			{
				// isn't possible to "undo" mission, so not required to clear relationship, also, the game already clears it during reload
				friendlyTriads = false;
				lastFriendlyTriads = friendlyTriads;
			}

			if (RIOT_TOTAL_PASSED_MISSIONS > 1)
			{
				if (!lastFriendlyLosAztecas)
				{
					friendlyLosAztecas = true;

					Command<COMMAND_SET_RELATIONSHIP>(1, ePedType::PED_TYPE_GANG2, ePedType::PED_TYPE_GANG8);
					Command<COMMAND_SET_RELATIONSHIP>(1, ePedType::PED_TYPE_GANG8, ePedType::PED_TYPE_GANG2);
					if (friendlyTriads)
					{
						Command<COMMAND_SET_RELATIONSHIP>(1, ePedType::PED_TYPE_GANG7, ePedType::PED_TYPE_GANG8);
						Command<COMMAND_SET_RELATIONSHIP>(1, ePedType::PED_TYPE_GANG8, ePedType::PED_TYPE_GANG7);
					}

					lastFriendlyLosAztecas = friendlyLosAztecas;
				}
			}
			else
			{
				friendlyLosAztecas = false;
				lastFriendlyLosAztecas = friendlyLosAztecas;
			}
		};


		// bad conversation
		injector::MakeInline<0x43B33C, 0x43B33C + 5>([](injector::reg_pack& regs)
		{
			//cmp     eax, 10h
			//jle     short loc_43B346
			int i = regs.eax;

			*(uintptr_t*)(regs.esp - 0x4) = 0x43B341; // false

			if (i <= ePedType::PED_TYPE_GANG10)
			{
				if ((friendlyLosAztecas && i == ePedType::PED_TYPE_GANG8) || (friendlyTriads && i != ePedType::PED_TYPE_GANG7))
				{
					//false
				}
				else
				{
					*(uintptr_t*)(regs.esp - 0x4) = 0x43B346; //true
				}
			}

		});

		// recruit
		injector::MakeInline<0x60C8AD, 0x60C8AD + 7>([](injector::reg_pack& regs)
		{
			//cmp     dword ptr [edi+598h], 8
			int i = *(DWORD*)(regs.edi + 0x598);

			if (i == ePedType::PED_TYPE_GANG2 || (friendlyTriads && i == ePedType::PED_TYPE_GANG7) || (friendlyLosAztecas && i == ePedType::PED_TYPE_GANG8))
			{
				*(uintptr_t*)(regs.esp - 0x4) = 0x60C8D5;
			}
			else
			{
				*(uintptr_t*)(regs.esp - 0x4) = 0x60C8B6;
			}
		});

		// recruit (again, because rockstar is dumb)
		injector::MakeInline<0x60D3F7, 0x60D3F7 + 7>([](injector::reg_pack& regs)
		{
			//cmp     dword ptr [ebp+598h], 8
			int i = *(DWORD*)(regs.ebp + 0x598);

			if (i == ePedType::PED_TYPE_GANG2 || (friendlyTriads && i == ePedType::PED_TYPE_GANG7) || (friendlyLosAztecas && i == ePedType::PED_TYPE_GANG8))
			{
				*(uintptr_t*)(regs.esp - 0x4) = 0x60D409;
			}
			else
			{
				*(uintptr_t*)(regs.esp - 0x4) = 0x60D400;
			}
		});
        
		// respond to cops
		injector::MakeInline<0x60D538, 0x60D538 + 7>([](injector::reg_pack& regs)
		{
			//cmp     dword ptr [esi+598h], 8
			int i = *(DWORD*)(regs.esi + 0x598);

			if (i == ePedType::PED_TYPE_GANG2 || (friendlyTriads && i == ePedType::PED_TYPE_GANG7) || (friendlyLosAztecas && i == ePedType::PED_TYPE_GANG8))
			{
				*(uintptr_t*)(regs.esp - 0x4) = 0x60D541;
			}
			else
			{
				*(uintptr_t*)(regs.esp - 0x4) = 0x60D592;
			}
		});
        
    }
} friendlyTriadsAndLosAztecasFix;
