// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ItemInstance.generated.h"


/**
 * Stores data used to initialize items.
 *
 * Can be subclassed to support different item types
 * (e.g., USwordItemData, UPotionItemData).
 *
 * Import methods to implement/override:
 *
 */
UCLASS(BlueprintType)
class UItemData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FText Name;

	UPROPERTY(EditDefaultsOnly)
	FText Description;

	UPROPERTY(EditDefaultsOnly)
	uint32 Value;

	//~ Begin UItemData contract
	virtual TSubclassOf<AActor> GetItemActorClass() const
	{
		return nullptr;
	}
	//~ EndUItemData contract
};

UCLASS(BlueprintType)
class USwordItemData : public UItemData
{
	GENERATED_BODY()

public:
	// World actor/representation
	// using static mesh actor for example purposes
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> ItemActor;

	// Damage
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sword|Damage")
	int32 BaseDamage;

	//UPROPERTY(BlueprintReadWrite, Category = "Sword|Damage")
	//int32 BonusDamage;

	// Critical strikes
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sword|Critical Strikes")
	float BaseCriticalStrikeChance;

	//UPROPERTY(BlueprintReadWrite, Category = "Sword|Critical Strikes")
	//float BonusCriticalStrikeChance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sword|Critical Strikes")
	float BaseCriticalStrikeMultiplier;

	/*UPROPERTY(BlueprintReadWrite, Category = "Sword|Critical Strikes")
	float BonusCriticalStrikeMultiplier;*/

	// Attack speed
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sword|Attack Speed")
	float BaseAttackSpeed;

	//UPROPERTY(BlueprintReadWrite, Category = "Sword|Attack Speed")
	//float BonusAttackSpeed;

	//~ Begin UItemData contract
	virtual TSubclassOf<AActor> GetItemActorClass() const override { return Cast<UClass>(ItemActor); }

	//~ EndUItemData contract
};

/**
 * Struct that associates an ItemInstance UClass
 * with all of the information needed to create
 * and initialize it.
 *
 * Such as the UItemData instance that should be
 * used to initialize it's properties, and the
 * Owner
 *
 *
 * I may have a USwordInstance class
 * but i can can initialize that class with many
 * different UItemData assets, e.g., "Fire Sword"
 * and "Steel Sword" use **different instances** of the
 * UItemData asset class, but they each use the same
 * UItemInstance class (e.g., USwordInstance).
 *
 * Reminder:
 *     - ItemClass is a Class pointer (not an instance)
 *     - ItemData is an object pointer (to an object instance)
 *
 * NOTE: Take care that the **class** of the UItemData
 * supports "binding" with the ItemClass
 * with.
 *
 * Example:
 *     ItemClass: "USwordInstance",
 *     ItemData: "USwordItemData"
 */
USTRUCT(BlueprintType)
struct FItemInstanceInitializer
{
	GENERATED_BODY()

	FItemInstanceInitializer()
	{
		// These need to be set before initializing an item instance with them
		Outer = nullptr;
		OwnerActor = nullptr;
	}

	/**
	 * The actor that will own the new ItemInstace
	 */
	UPROPERTY(BlueprintReadWrite)
	AActor* OwnerActor;

	/**
	 * The outer to use for the ItemInstance, can be the same object as the OwnerActor
	 */
	UPROPERTY(BlueprintReadWrite)
	UObject* Outer;

	/**
	 * @brief Any item class that you want to instantiate and initialize
	 * with ItemData.
	 *
	 * Example: "USwordInstance", "UConsumableInstance", "UCraftingMatInstance"
	 */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UItemInstance> ItemClass;

	/**
	 * @brief Any UItemData which supports initializing the ItemClass
	 *
	 * Example configurations:
	 *    ItemClass: "USwordInstance", ItemData: "USwordItemData"
	 *    ItemClass: "UConsumableInstance", ItemData: "UPotionItemData"
	 *    ItemClass: "UCraftingMatInstance", ItemData: "UGearGemItemData"
	 */
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UItemData> ItemData;
};


/**
 * Runtime representation of an item.
 *
 * Instantiation of this class should happen
 * solely through the CreateItemInstance method.
 */
UCLASS(BlueprintType, Abstract)
class INVTEST_API UItemInstance : public UObject
{
	GENERATED_BODY()

public:
	//~ Begin UObject Interface.
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool IsSupportedForNetworking() const override { return true; }
	//~ End UObject Interface.

	/**
	 * Creates a new item instance and initializes it.
	 *
	 * Note: This method should be called from VERY few places.
	 *
	 * If this is executed on client trigger an exception.
	 */
	UFUNCTION(BlueprintCallable)
	static UItemInstance* CreateItemInstance(const FItemInstanceInitializer& ItemInitializer);


protected:
	friend class UInventoryComponent;
	/**
	 * @brief All data needed for a particular UItemInstance subclass to function.
	 *
	 * For example, USwordInstance might need to store floats for its damage, a mesh
	 * for the sword, etc.
	 */
	UPROPERTY(BlueprintReadWrite, Category = "Item|Data")
	TObjectPtr<UItemData> Data;

	/**
	 * @brief Pointer to the actor that logically owns this instance
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Item|Ownership")
	AActor* OwnerActor;

	//--------------------------------------------
	// Item actors
	//--------------------------------------------
	/**
	 * @brief Overridable function that can be used to implement creating actors/world
	 * representations of item instances. For example, spawning a mesh in the world to
	 * represent a sword.
	 *
	 */
	virtual AActor* TrySpawnItemActor();

	/**
	 * @brief Overridable function that can be used to implement destroying spawned
	 * actors that were spawned with SpawnItemActor()
	 *
	 * Returns true if successfully destroyed, false if not.
	 */
	virtual bool TryDestroyItemActor();
private:
	virtual AActor* InternalSpawnItemActor();
	virtual bool InternalDestroyItemActor();
private:
	//--------------------------------------------
	// Item actor spawn condition hook methods
	//--------------------------------------------
	/**
	 * @brief Overridable function that can be used to implement custom
	 * spawning behaviors/conditions.
	 *
	 * For example, you might not want an ItemActor to spawn when the player's
	 * ASC has the "Debuff.Stunned" gameplay tag.
	 *
	 * @return true if the item can be spawned, false if not
	 */
	virtual bool CanSpawnItemActor();
	virtual bool CanDestroyItemActor();
private:
	/**
	 * @brief Invoked when the item actor is destroyed.
	 *
	 * Note: This method is bound to the OnDestroyed delegate of the actor, so it will be implicitly executed whenever
	 * the actor is destroyed.
	 */
	UFUNCTION()
	virtual void HandleItemActorDestroyed(AActor* InActor);
private:
	UPROPERTY(Replicated)
	TObjectPtr<AActor> ItemActor;
};
