#include "game/formation/FormationPlanner.h"
#include "tests/TestFramework.h"

using namespace bge;

namespace
{
    static Unit MakeUnit(Entity entity, const Vec2& position)
    {
        Unit unit(entity);
        unit.movement.position = position;
        unit.movement.velocity = Vec2::Zero();
        unit.movement.desiredVelocity = Vec2::Zero();
        unit.movement.target = MovementTarget(position);
        unit.movement.maxSpeed = Scalar::FromInt(1);
        unit.movement.radius = Scalar::FromRaw(Scalar::OneRaw / 2);
        unit.movement.hasTarget = false;
        return unit;
    }
}

BGE_TEST(FormationPlannerTests, EmptyInputProducesEmptyPlan)
{
    const std::vector<Unit*> units;
    const FormationPlan plan =
        FormationPlanner::BuildMoveFormation(
            units,
            Vec2(Scalar::Zero(), Scalar::Zero()),
            Scalar::One());

    BGE_TEST_TRUE(plan.IsEmpty());
    BGE_TEST_TRUE(plan.layout.IsEmpty());
    BGE_TEST_EQ(plan.AssignmentCount(), 0U);
}

BGE_TEST(FormationPlannerTests, SingleUnitProducesSingleAssignment)
{
    Unit unit = MakeUnit(
        Entity(0U, 0U),
        Vec2(Scalar::Zero(), Scalar::Zero()));

    std::vector<Unit*> units;
    units.push_back(&unit);

    const Vec2 center(Scalar::FromInt(10), Scalar::FromInt(5));

    const FormationPlan plan =
        FormationPlanner::BuildMoveFormation(
            units,
            center,
            Scalar::One());

    BGE_TEST_EQ(plan.AssignmentCount(), 1U);
    BGE_TEST_EQ(plan.layout.SlotCount(), 1U);
    BGE_TEST_EQ(plan.assignments[0].entity, unit.entity);
    BGE_TEST_EQ(plan.assignments[0].targetPosition, center);
}

BGE_TEST(FormationPlannerTests, MultipleUnitsProduceMatchingAssignmentCount)
{
    Unit a = MakeUnit(Entity(0U, 0U), Vec2(Scalar::FromInt(0), Scalar::FromInt(0)));
    Unit b = MakeUnit(Entity(1U, 0U), Vec2(Scalar::FromInt(1), Scalar::FromInt(0)));
    Unit c = MakeUnit(Entity(2U, 0U), Vec2(Scalar::FromInt(2), Scalar::FromInt(0)));
    Unit d = MakeUnit(Entity(3U, 0U), Vec2(Scalar::FromInt(3), Scalar::FromInt(0)));

    std::vector<Unit*> units;
    units.push_back(&a);
    units.push_back(&b);
    units.push_back(&c);
    units.push_back(&d);

    const FormationPlan plan =
        FormationPlanner::BuildMoveFormation(
            units,
            Vec2(Scalar::FromInt(20), Scalar::FromInt(10)),
            Scalar::One());

    BGE_TEST_EQ(plan.AssignmentCount(), 4U);
    BGE_TEST_EQ(plan.layout.SlotCount(), 4U);
}

BGE_TEST(FormationPlannerTests, AssignmentsUseUniqueSlots)
{
    Unit a = MakeUnit(Entity(0U, 0U), Vec2(Scalar::FromInt(0), Scalar::FromInt(0)));
    Unit b = MakeUnit(Entity(1U, 0U), Vec2(Scalar::FromInt(1), Scalar::FromInt(0)));
    Unit c = MakeUnit(Entity(2U, 0U), Vec2(Scalar::FromInt(2), Scalar::FromInt(0)));

    std::vector<Unit*> units;
    units.push_back(&a);
    units.push_back(&b);
    units.push_back(&c);

    const FormationPlan plan =
        FormationPlanner::BuildMoveFormation(
            units,
            Vec2(Scalar::FromInt(10), Scalar::FromInt(10)),
            Scalar::One());

    BGE_TEST_EQ(plan.AssignmentCount(), 3U);

    for (usize i = 0; i < plan.assignments.size(); ++i)
    {
        for (usize j = i + 1; j < plan.assignments.size(); ++j)
        {
            BGE_TEST_TRUE(
                plan.assignments[i].targetPosition !=
                plan.assignments[j].targetPosition);
        }
    }
}

BGE_TEST(FormationPlannerTests, NearestSlotAssignmentWorksInSimpleCase)
{
    Unit left = MakeUnit(
        Entity(0U, 0U),
        Vec2(Scalar::FromInt(-10), Scalar::Zero()));

    Unit right = MakeUnit(
        Entity(1U, 0U),
        Vec2(Scalar::FromInt(10), Scalar::Zero()));

    std::vector<Unit*> units;
    units.push_back(&left);
    units.push_back(&right);

    const FormationPlan plan =
        FormationPlanner::BuildMoveFormation(
            units,
            Vec2(Scalar::Zero(), Scalar::Zero()),
            Scalar::FromInt(2));

    BGE_TEST_EQ(plan.AssignmentCount(), 2U);

    const Vec2 leftTarget = plan.assignments[0].entity == left.entity
        ? plan.assignments[0].targetPosition
        : plan.assignments[1].targetPosition;

    const Vec2 rightTarget = plan.assignments[0].entity == right.entity
        ? plan.assignments[0].targetPosition
        : plan.assignments[1].targetPosition;

    BGE_TEST_TRUE(leftTarget.x <= rightTarget.x);
}

BGE_TEST(FormationPlannerTests, InvalidUnitsAreIgnored)
{
    Unit valid = MakeUnit(
        Entity(0U, 0U),
        Vec2(Scalar::Zero(), Scalar::Zero()));

    Unit invalid;
    invalid.entity = Entity::Invalid();

    std::vector<Unit*> units;
    units.push_back(&valid);
    units.push_back(&invalid);

    const FormationPlan plan =
        FormationPlanner::BuildMoveFormation(
            units,
            Vec2(Scalar::FromInt(5), Scalar::FromInt(5)),
            Scalar::One());

    BGE_TEST_EQ(plan.AssignmentCount(), 1U);
    BGE_TEST_EQ(plan.assignments[0].entity, valid.entity);
}

BGE_TEST(FormationPlannerTests, NullUnitsAreIgnored)
{
    Unit valid = MakeUnit(
        Entity(0U, 0U),
        Vec2(Scalar::Zero(), Scalar::Zero()));

    std::vector<Unit*> units;
    units.push_back(&valid);
    units.push_back(nullptr);

    const FormationPlan plan =
        FormationPlanner::BuildMoveFormation(
            units,
            Vec2(Scalar::FromInt(5), Scalar::FromInt(5)),
            Scalar::One());

    BGE_TEST_EQ(plan.AssignmentCount(), 1U);
    BGE_TEST_EQ(plan.assignments[0].entity, valid.entity);
}

BGE_TEST(FormationPlannerTests, SameInputProducesDeterministicAssignments)
{
    Unit a1 = MakeUnit(Entity(0U, 0U), Vec2(Scalar::FromInt(0), Scalar::FromInt(0)));
    Unit b1 = MakeUnit(Entity(1U, 0U), Vec2(Scalar::FromInt(4), Scalar::FromInt(0)));
    Unit c1 = MakeUnit(Entity(2U, 0U), Vec2(Scalar::FromInt(8), Scalar::FromInt(0)));

    Unit a2 = MakeUnit(Entity(0U, 0U), Vec2(Scalar::FromInt(0), Scalar::FromInt(0)));
    Unit b2 = MakeUnit(Entity(1U, 0U), Vec2(Scalar::FromInt(4), Scalar::FromInt(0)));
    Unit c2 = MakeUnit(Entity(2U, 0U), Vec2(Scalar::FromInt(8), Scalar::FromInt(0)));

    std::vector<Unit*> units1;
    units1.push_back(&a1);
    units1.push_back(&b1);
    units1.push_back(&c1);

    std::vector<Unit*> units2;
    units2.push_back(&a2);
    units2.push_back(&b2);
    units2.push_back(&c2);

    const FormationPlan plan1 =
        FormationPlanner::BuildMoveFormation(
            units1,
            Vec2(Scalar::FromInt(10), Scalar::FromInt(10)),
            Scalar::FromInt(2));

    const FormationPlan plan2 =
        FormationPlanner::BuildMoveFormation(
            units2,
            Vec2(Scalar::FromInt(10), Scalar::FromInt(10)),
            Scalar::FromInt(2));

    BGE_TEST_EQ(plan1.AssignmentCount(), plan2.AssignmentCount());

    for (usize i = 0; i < plan1.assignments.size(); ++i)
    {
        BGE_TEST_EQ(plan1.assignments[i].entity, plan2.assignments[i].entity);
        BGE_TEST_EQ(plan1.assignments[i].targetPosition, plan2.assignments[i].targetPosition);
    }
}