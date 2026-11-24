# FirstGame

Unreal Engine 5.8. Проект на основе шаблона Third Person, расширенный C++ классом персонажа.

## Сделано

- **Наследование.** C++ класс `AMyCharacter` (наследник `ACharacter`). Родитель `BP_ThirdPersonCharacter` заменён на него (File -> Reparent Blueprint), движение, камера и прыжок из шаблона работают как раньше.
- **Enhanced Input.** Созданы `IA_Sprint` (Left Shift) и `IA_Interact` (E) и добавлены в `IMC_Default`. Привязка через `BindAction` в `SetupPlayerInputComponent`. Ассеты назначены в Class Defaults `BP_ThirdPersonCharacter` (поля `SprintAction` и `InteractAction`).
- **Делегат.** `OnInteract` (Dynamic Multicast Delegate): вызывается (`Broadcast`) при нажатии E. В `BP_ThirdPersonCharacter` на него подписка (Bind Event to On Interact в Event BeginPlay), обработчик `HandleInteract` выводит `Interaction triggered!` через Print String.
- **Спринт.** При зажатом Left Shift `MaxWalkSpeed` увеличивается до `SprintSpeed` (1000), при отпускании возвращается обычная скорость. Состояние хранится в `bIsSprinting` (`UPROPERTY(BlueprintReadOnly)`).
- **Animation Blueprint.** В `ABP_Unarmed` добавлена переменная `bIsSprinting` (обновляется в Event Graph через Cast To MyCharacter), в State Machine добавлено состояние `Sprint` (анимация `MF_Unarmed_Jog_Fwd`, зациклена) с переходами `Locomotion <-> Sprint`. Переход в Sprint: `bIsSprinting AND ShouldMove`, обратно: `NOT (bIsSprinting AND ShouldMove)`. Условие `ShouldMove` нужно, чтобы при зажатом Shift на месте не проигрывалась анимация бега на месте.
- **Stamina.**
  - Переменная `Stamina` (`UPROPERTY(BlueprintReadOnly)`), максимум 100 (`MaxStamina`).
  - В `Tick`: пока идёт спринт и персонаж двигается, выносливость тратится (`StaminaDrainRate * DeltaTime`), иначе восстанавливается (`StaminaRegenRate * DeltaTime`) до максимума.
  - При `Stamina <= 0` вызывается `StopSprint()`, даже если Shift зажат.
  - `StartSprint` не запускает бег, если `Stamina` меньше `MinStaminaToSprint` (10).
  - Значение выводится на экран через `GEngine->AddOnScreenDebugMessage` (с проверкой `GEngine`).

## Управление

| Клавиша    | Действие                                          |
| ---------- | ------------------------------------------------- |
| W A S D    | движение                                          |
| Мышь       | поворот камеры                                    |
| Space      | прыжок                                            |
| Left Shift | спринт                                            |
| E          | взаимодействие (надпись `Interaction triggered!`) |

## Параметры (можно менять в Class Defaults `BP_ThirdPersonCharacter`)

| Параметр             | Значение по умолчанию | Назначение                               |
| -------------------- | --------------------- | ---------------------------------------- |
| `SprintSpeed`        | 1000                  | скорость во время спринта                |
| `MaxStamina`         | 100                   | максимум выносливости                    |
| `StaminaDrainRate`   | 25                    | трата выносливости в секунду при спринте |
| `StaminaRegenRate`   | 15                    | восстановление в секунду                 |
| `MinStaminaToSprint` | 10                    | минимум для начала спринта               |

## ДЗ2 - лес, PCG и ИИ

Стартовая карта: `Content/Maps/Lvl_Forest` (туманный лес, ландшафт, NavMesh).

### Уровень и PCG

- Ландшафт + туман / приглушённый свет, земля - материал на текстуре ambientCG Ground037 (CC0).
- Деревья расставляются через PCG (`Content/PCG/PCG_Forest` + PCG Volume на уровне). Модели елей с Poly Haven (CC0), у стволов упрощённая коллизия, чтобы игрок и ИИ не проходили насквозь.
- `NavMeshBoundsVolume`, Runtime Generation = Dynamic. Клавиша `P` во вьюпорте показывает сетку.
- Для большей атмосферы уровня задумано: `Fog Height Falloff = 0.1` в `ExponentialHeightFog` и в `PCG/PCG_Forest` у семплера поверхности `Точек на квадратный метр = 0.03` (так было изначально; для удобства слежения за врагами параметры временно опущены).

### ИИ

- C++ `AEnemyCharacter` + `AEnemyAIController` (`AAIController`). Контроллер только пишет факты в Blackboard (зрение -> `TargetActor`), решения принимает Behavior Tree - не в обход дерева.
- Ассеты: `Content/AI/BB_Enemy`, `BT_Enemy`, `BP_Enemy`. На уровне стоят несколько `BP_Enemy`.
- Дерево (приоритет слева направо):
  1. **Chase** - если задан `TargetActor`: быстрее идёт к игроку (`Move To`).
  2. **Suspicion** - бонус, см. ниже.
  3. **Patrol** - ходит вокруг `HomeLocation` (точка спавна).
- Зрение: `AIPerception` Sight. Радиусы настраиваются в `BP_Enemy` (категории AI | Sight).
- Кастомные таски для базы: `Find Patrol Location`, `Set Movement Speed`.

Враги патрулируют и обходят деревья по NavMesh, при обнаружении игрока переключаются на погоню, после потери цели возвращаются к патрулю.

### Бонус на 9-10: состояние «Подозрение»

Реализовано поведение из задания: пока ИИ игрока не видит для погони, но тот ещё относительно близко, враг не стоит на месте и не сразу преследует - он **ищет вокруг игрока**. Чем дальше игрок от врага, тем **больше радиус** поиска.

Как устроено:

1. Service `Update Suspicion` (кастомный `UBTService`) раз в ~0.25 с:
   - если есть `TargetActor` (погоня) - подозрение сбрасывается;
   - иначе если дистанция до игрока <= `SuspicionRange` - в Blackboard пишется `SuspectActor` (игрок);
   - если дальше - `SuspectActor` очищается, снова обычный патруль.
2. Task `Find Suspicion Location` (кастомный `UBTTaskNode`):
   - берёт позицию игрока из `SuspectActor`;
   - радиус точки = `map(дистанция, SuspicionMinDistance..SuspicionRange -> SuspicionMinRadius..SuspicionMaxRadius)`;
   - пишет результат в `MoveLocation`, дальше обычный `Move To`.
3. Ветка **Suspicion** в `BT_Enemy` между Chase и Patrol (декоратор `SuspectActor Is Set`).

Над головой показывается `?` (подозрение) и `!` (погоня) через `StateText`. Галочка **Show State Text** в `BP_Enemy` сейчас включена для удобства; её можно выключить, если индикатор не нужен.

### Параметры ИИ (Class Defaults / инстанс `BP_Enemy`)

| Параметр | По умолчанию | Назначение |
| --- | --- | --- |
| `SightRadius` | 1200 | с какого расстояния начинается погоня |
| `LoseSightRadius` | 1500 | когда теряет цель |
| `SightAngleDegrees` | 180 | угол зрения (180 = кругом) |
| `SuspicionRange` | 2000 | до какой дистанции действует подозрение |
| `SuspicionMinDistance` | 1200 | ближний край для расчёта радиуса поиска |
| `SuspicionMinRadius` | 300 | узкий круг поиска, когда игрок близко |
| `SuspicionMaxRadius` | 1000 | широкий круг, когда игрок дальше |
| `bShowStateText` | true | показать `?` / `!` над головой (можно выключить) |

Скорости Chase / Suspicion / Patrol задаются в узлах **Set Movement Speed** внутри `BT_Enemy`.
