#include "SDK/sdk.h"

int main(void)
{
	sdk* cs2 = new sdk();
	if (cs2->IsElevated())
	{
		logger::success("User is elevated!");
		MessageBoxA(NULL, "This process is elvated!", "INFO", MB_OK);
	}
	else
	{
		logger::error("User is not elvated run as admin!", false);
		MessageBoxA(NULL, "This process is NOT elvated!", "INFO", MB_ICONERROR | MB_OK);
		cout << "bypassed";
		cin.get();
	}
	cs2->init();
}