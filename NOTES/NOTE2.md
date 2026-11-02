# UML Class Relationships

A UML (Unified Modeling Language) class diagram represents the static structure of a system, showing its classes, attributes, methods, and relationships between classes.

For exams, focus on these five concepts:

1. Association
2. Aggregation
3. Composition
4. Inheritance
5. Multiplicity

## 1. Association

**Definition:** Association is a relationship between two independent classes that interact with or are connected to each other.

**Example:** Teacher and Student. Both exist on their own.

**UML:** a solid line (an arrow shows the direction, if it is one-way).

## 2. Aggregation

**Definition:** Aggregation is a weak “has-a” relationship in which one class contains or groups objects of another class, but those objects can exist independently.

**Example:** Department and Professor. If the department is closed, the professors still exist.

**UML:** a solid line with a hollow (empty) diamond on the whole's side.

**In code:** the whole holds a pointer or reference to a part that was created outside it.

## 3. Composition

**Definition:** Composition is a strong “has-a” relationship in which the part's lifecycle is controlled by the whole. If the whole is destroyed, its parts normally cannot continue as parts of that whole.

**Example:** House and Room. If the house is destroyed, its rooms are destroyed too.

**UML:** a solid line with a filled (black) diamond on the whole's side.

**In code:** the whole creates and owns the part, usually as a member object.

## 4. Inheritance (Generalization)

**Definition:** Inheritance represents an “is-a” relationship in which a child class inherits properties and methods from a parent class.

**Example:** Dog is an Animal.

**UML:** a solid line with a hollow triangle pointing to the parent class.

## 5. Multiplicity (Cardinality)

**Definition:** Multiplicity specifies how many instances of one class can be associated with an instance of another class.

**Example:** one Customer can place many Orders, and each Order belongs to exactly one Customer.

**UML:** written at the ends of the line: `1` (exactly one), `0..1` (zero or one), `*` (zero or more), `1..*` (one or more).

## Quick Revision

| Relationship | Meaning | UML symbol | Example |
| --- | --- | --- | --- |
| Association | uses / knows about | solid line | Teacher – Student |
| Aggregation | weak has-a, part lives on its own | hollow diamond | Department – Professor |
| Composition | strong has-a, part dies with the whole | filled diamond | House – Room |
| Inheritance | is-a | hollow triangle | Dog → Animal |
| Multiplicity | how many | `1`, `0..1`, `*`, `1..*` | Customer 1 – * Order |
