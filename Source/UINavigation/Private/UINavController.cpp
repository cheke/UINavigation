// Copyright (C) 2018 Gon�alo Marques - All Rights Reserved

#include "UINavController.h"
#include "UINavWidget.h"


void AUINavController::SetupInputComponent()
{
	Super::SetupInputComponent();

	FInputActionBinding& Action1_1 = InputComponent->BindAction("MenuUp", IE_Pressed, this, &AUINavController::StartMenuUp);
	Action1_1.bExecuteWhenPaused = true;
	Action1_1.bConsumeInput = false;
	FInputActionBinding& Action2_1 = InputComponent->BindAction("MenuDown", IE_Pressed, this, &AUINavController::StartMenuDown);
	Action2_1.bExecuteWhenPaused = true;
	Action2_1.bConsumeInput = false;
	FInputActionBinding& Action3_1 = InputComponent->BindAction("MenuLeft", IE_Pressed, this, &AUINavController::StartMenuLeft);
	Action3_1.bExecuteWhenPaused = true;
	Action3_1.bConsumeInput = false;
	FInputActionBinding& Action4_1 = InputComponent->BindAction("MenuRight", IE_Pressed, this, &AUINavController::StartMenuRight);
	Action4_1.bExecuteWhenPaused = true;
	Action4_1.bConsumeInput = false;
	FInputActionBinding& Action5_1 = InputComponent->BindAction("MenuSelect", IE_Pressed, this, &AUINavController::MenuSelect);
	Action5_1.bExecuteWhenPaused = true;
	Action5_1.bConsumeInput = false;
	FInputActionBinding& Action6_1 = InputComponent->BindAction("MenuReturn", IE_Pressed, this, &AUINavController::MenuReturn);
	Action6_1.bExecuteWhenPaused = true;
	Action6_1.bConsumeInput = false;

	FInputActionBinding& Action1_2 = InputComponent->BindAction("MenuUp", IE_Released, this, &AUINavController::MenuUpRelease);
	Action1_2.bExecuteWhenPaused = true;
	Action1_2.bConsumeInput = false;
	FInputActionBinding& Action2_2 = InputComponent->BindAction("MenuDown", IE_Released, this, &AUINavController::MenuDownRelease);
	Action2_2.bExecuteWhenPaused = true;
	Action2_2.bConsumeInput = false;
	FInputActionBinding& Action3_2 = InputComponent->BindAction("MenuLeft", IE_Released, this, &AUINavController::MenuLeftRelease);
	Action3_2.bExecuteWhenPaused = true;
	Action3_2.bConsumeInput = false;
	FInputActionBinding& Action4_2 = InputComponent->BindAction("MenuRight", IE_Released, this, &AUINavController::MenuRightRelease);
	Action4_2.bExecuteWhenPaused = true;
	Action4_2.bConsumeInput = false;
}

void AUINavController::Possess(APawn * InPawn)
{
	Super::Possess(InPawn);

	FetchUINavActionKeys();
}

void AUINavController::FetchUINavActionKeys()
{
	const UInputSettings* Settings = GetDefault<UInputSettings>();
	const TArray<FInputActionKeyMapping>& Actions = Settings->ActionMappings;
	for (FInputActionKeyMapping Action : Actions)
	{
		FString NewName = Action.ActionName.ToString();
		if (NewName.Left(4).Compare(TEXT("Menu")) != 0)
			continue;

		TArray<FKey>* KeyArray = KeyMap.Find(NewName);
		if (KeyArray == nullptr)
		{
			KeyMap.Add(NewName, { Action.Key });
		}
		else
		{
			KeyArray->Add(Action.Key);
		}
	}

}

void AUINavController::SetActiveWidget(UUINavWidget* NewWidget)
{
	ActiveWidget = NewWidget;
}

EInputType AUINavController::GetKeyInputType(FKey Key)
{
	if (Key.IsGamepadKey())
	{
		return EInputType::Gamepad;
	}
	else if (Key.IsMouseButton())
	{
		return EInputType::Mouse;
	}
	else
	{
		return EInputType::Keyboard;
	}

	return CurrentInputType;
}

EInputType AUINavController::GetActionInputType(FString Action)
{
	TArray<FString> Actions;
	KeyMap.GenerateKeyArray(Actions);
	for (FString action : Actions)
	{
		for (FKey key : KeyMap[action])
		{
			if (WasInputKeyJustPressed(key))
			{
				if (key.IsGamepadKey())
				{
					return EInputType::Gamepad;
				}
				else if (key.IsMouseButton())
				{
					return EInputType::Mouse;
				}
				else
				{
					return EInputType::Keyboard;
				}
			}
		}
	}
	return CurrentInputType;
}

void AUINavController::VerifyInputTypeChangeByKey(FKey Key)
{
	EInputType NewInputType = GetKeyInputType(Key);

	if (NewInputType != CurrentInputType)
	{
		NotifyInputTypeChange(NewInputType);
	}
}

void AUINavController::VerifyInputTypeChangeByAction(FString Action)
{
	EInputType NewInputType = GetActionInputType(Action);

	if (NewInputType != CurrentInputType)
	{
		NotifyInputTypeChange(NewInputType);
	}
}

void AUINavController::NotifyKeyPressed(FKey PressedKey)
{
	FindActionByKey(PressedKey, true);
}

void AUINavController::NotifyKeyReleased(FKey ReleasedKey)
{
	FindActionByKey(ReleasedKey, false);
}

void AUINavController::FindActionByKey(FKey PressedKey, bool bPressed)
{
	TArray<FString> Actions;
	KeyMap.GenerateKeyArray(Actions);
	for (FString action : Actions)
	{
		for (FKey key : KeyMap[action])
		{
			if (key == PressedKey)
			{
				VerifyInputTypeChangeByKey(PressedKey);
				ExecuteActionByName(action, bPressed);
				return;
			}
		}
	}
}

void AUINavController::ExecuteActionByName(FString Action, bool bPressed)
{
	if (Action.Equals("MenuUp"))
	{
		if (bPressed)
		{
			StartMenuUp();
		}
		else
		{
			MenuUpRelease();
		}
	}
	else if (Action.Equals("MenuDown"))
	{
		if (bPressed)
		{
			StartMenuDown();
		}
		else
		{
			MenuDownRelease();
		}
	}
	else if (Action.Equals("MenuLeft"))
	{
		if (bPressed)
		{
			StartMenuLeft();
		}
		else
		{
			MenuLeftRelease();
		}
	}
	else if (Action.Equals("MenuRight"))
	{
		if (bPressed)
		{
			StartMenuRight();
		}
		else
		{
			MenuRightRelease();
		}
	}
	else if (Action.Equals("MenuSelect"))
	{
		if (bPressed)
		{
			MenuSelect();
		}
	}
	else if (Action.Equals("MenuReturn"))
	{
		if (bPressed)
		{
			MenuReturn();
		}
	}
}

void AUINavController::NotifyMouseInputType()
{
	EInputType NewInputType = EInputType::Mouse;

	if (NewInputType != CurrentInputType)
	{
		NotifyInputTypeChange(NewInputType);
	}
}

void AUINavController::NotifyInputTypeChange(EInputType NewInputType)
{
	GetWorldTimerManager().ClearTimer(NavChainHandle);

	if (ActiveWidget == nullptr) return;
	ActiveWidget->OnInputChanged(CurrentInputType, NewInputType);
	CurrentInputType = NewInputType;
}

bool AUINavController::IsGamepadConnected()
{
	return FSlateApplication::Get().IsGamepadAttached();
}

void AUINavController::OnRootWidgetRemoved_Implementation()
{

}

void AUINavController::MenuUp()
{
	if (ActiveWidget == nullptr ||
		!ActiveWidget->IsInViewport()) return;

	VerifyInputTypeChangeByAction(TEXT("MenuUp"));
	ActiveWidget->MenuUp();
}

void AUINavController::MenuDown()
{
	if (ActiveWidget == nullptr ||
		!ActiveWidget->IsInViewport()) return;

	VerifyInputTypeChangeByAction(TEXT("MenuDown"));
	ActiveWidget->MenuDown();
}

void AUINavController::MenuLeft()
{
	if (ActiveWidget == nullptr ||
		!ActiveWidget->IsInViewport()) return;

	VerifyInputTypeChangeByAction(TEXT("MenuLeft"));
	ActiveWidget->MenuLeft();
}

void AUINavController::MenuRight()
{
	if (ActiveWidget == nullptr ||
		!ActiveWidget->IsInViewport()) return;

	VerifyInputTypeChangeByAction(TEXT("MenuRight"));
	ActiveWidget->MenuRight();
}

void AUINavController::MenuSelect()
{
	if (ActiveWidget == nullptr ||
		!ActiveWidget->IsInViewport()) return;

	GetWorldTimerManager().ClearTimer(NavChainHandle);
	VerifyInputTypeChangeByAction(TEXT("MenuSelect"));
	ActiveWidget->MenuSelect();
}

void AUINavController::MenuReturn()
{
	if (ActiveWidget == nullptr ||
		!ActiveWidget->IsInViewport()) return;

	GetWorldTimerManager().ClearTimer(NavChainHandle);
	VerifyInputTypeChangeByAction(TEXT("MenuReturn"));
	ActiveWidget->MenuReturn();
}

void AUINavController::MenuUpRelease()
{
	if (Direction != EInputDirection::Up) return;

	GetWorldTimerManager().ClearTimer(NavChainHandle);
}

void AUINavController::MenuDownRelease()
{
	if (Direction != EInputDirection::Down) return;

	GetWorldTimerManager().ClearTimer(NavChainHandle);
}

void AUINavController::MenuLeftRelease()
{
	if (Direction != EInputDirection::Left) return;

	GetWorldTimerManager().ClearTimer(NavChainHandle);
}

void AUINavController::MenuRightRelease()
{
	if (Direction != EInputDirection::Right) return;

	GetWorldTimerManager().ClearTimer(NavChainHandle);
}

void AUINavController::StartMenuUp()
{
	if (ActiveWidget == nullptr ||
		!ActiveWidget->IsInViewport()) return;

	MenuUp();
	Direction = EInputDirection::Up;

	if (!bChainNavigation) return;

	GetWorldTimerManager().SetTimer(NavChainHandle, this, &AUINavController::MenuUp, NavigationChainFrequency, true, InputHeldWaitTime);
}

void AUINavController::StartMenuDown()
{
	if (ActiveWidget == nullptr ||
		!ActiveWidget->IsInViewport()) return;

	MenuDown();
	Direction = EInputDirection::Down;

	if (!bChainNavigation) return;
	GetWorldTimerManager().SetTimer(NavChainHandle, this, &AUINavController::MenuDown, NavigationChainFrequency, true, InputHeldWaitTime);
}

void AUINavController::StartMenuLeft()
{
	if (ActiveWidget == nullptr ||
		!ActiveWidget->IsInViewport()) return;

	MenuLeft();
	Direction = EInputDirection::Left;

	if (!bChainNavigation) return;
	GetWorldTimerManager().SetTimer(NavChainHandle, this, &AUINavController::MenuLeft, NavigationChainFrequency, true, InputHeldWaitTime);
}

void AUINavController::StartMenuRight()
{
	if (ActiveWidget == nullptr ||
		!ActiveWidget->IsInViewport()) return;

	MenuRight();
	Direction = EInputDirection::Right;

	if (!bChainNavigation) return;
	GetWorldTimerManager().SetTimer(NavChainHandle, this, &AUINavController::MenuRight, NavigationChainFrequency, true, InputHeldWaitTime);
}