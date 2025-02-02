// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{

	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;
}

void UInventoryComponent::ServerCreateItemInInventory_Implementation(TSubclassOf<UItemInstance> ItemClass, UItemData* ItemData)
{
	CreateItemInInventory(ItemClass, ItemData);
}


void UInventoryComponent::OnRep_Items()
{
	UE_LOG(LogTemp, Log, TEXT("OnRep_Items!"));

}

