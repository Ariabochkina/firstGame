#include "AI/BTService_UpdateSuspicion.h"

#include "AI/AIBlackboardKeys.h"
#include "AI/EnemyCharacter.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

UBTService_UpdateSuspicion::UBTService_UpdateSuspicion()
{
	NodeName = TEXT("Update Suspicion");
	Interval = 0.25f;
	RandomDeviation = 0.0f;
	bCallTickOnSearchStart = true;
	INIT_SERVICE_NODE_NOTIFY_FLAGS();

	TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_UpdateSuspicion, TargetActorKey), AActor::StaticClass());
	SuspectActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_UpdateSuspicion, SuspectActorKey), AActor::StaticClass());
	TargetActorKey.SelectedKeyName = AIBlackboard::TargetActor;
	SuspectActorKey.SelectedKeyName = AIBlackboard::SuspectActor;
}

void UBTService_UpdateSuspicion::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	AAIController* Controller = OwnerComp.GetAIOwner();
	APawn* Self = Controller ? Controller->GetPawn() : nullptr;
	if (!BlackboardComp || !Self)
	{
		return;
	}

	AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(Self);
	APawn* Player = UGameplayStatics::GetPlayerPawn(Self, 0);
	const bool bChasing = BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName) != nullptr;
	const float Distance = Player
		? static_cast<float>(FVector::Dist(Self->GetActorLocation(), Player->GetActorLocation()))
		: TNumericLimits<float>::Max();
	const float Range = Enemy ? Enemy->SuspicionRange : 2000.0f;

	if (bChasing)
	{
		BlackboardComp->ClearValue(SuspectActorKey.SelectedKeyName);
		if (Enemy)
		{
			Enemy->SetStateText(TEXT("!"));
		}
	}
	else if (Player && Distance <= Range)
	{
		BlackboardComp->SetValueAsObject(SuspectActorKey.SelectedKeyName, Player);
		if (Enemy)
		{
			Enemy->SetStateText(TEXT("?"));
		}
	}
	else
	{
		BlackboardComp->ClearValue(SuspectActorKey.SelectedKeyName);
		if (Enemy)
		{
			Enemy->SetStateText(TEXT(""));
		}
	}

	if (bDrawDebug)
	{
		DrawDebugSphere(Self->GetWorld(), Self->GetActorLocation(), Range, 32, FColor::Yellow, false, Interval);
	}
}
