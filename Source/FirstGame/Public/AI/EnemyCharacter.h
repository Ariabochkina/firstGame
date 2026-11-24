#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

class UBehaviorTree;
class UTextRenderComponent;

UCLASS()
class FIRSTGAME_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemyCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Sight", meta = (ClampMin = "0.0"))
	float SightRadius = 1200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Sight", meta = (ClampMin = "0.0"))
	float LoseSightRadius = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Sight", meta = (ClampMin = "0.0", ClampMax = "180.0"))
	float SightAngleDegrees = 180.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Suspicion", meta = (ClampMin = "0.0"))
	float SuspicionRange = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Suspicion", meta = (ClampMin = "0.0"))
	float SuspicionMinDistance = 1200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Suspicion", meta = (ClampMin = "0.0"))
	float SuspicionMinRadius = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Suspicion", meta = (ClampMin = "0.0"))
	float SuspicionMaxRadius = 1000.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UTextRenderComponent> StateText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	bool bShowStateText = false;

	void SetStateText(const FString& Text);
};
