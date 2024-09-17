# GOBP
GOBP - Goal Oriented Behaviour Planning. Runtime procedureal Behaviour Tree creationg using GOAP

*Will have a more in-depth post about this code in my personal blogpost later*

Link to the blogpost: [Blog Post](https://farukosm.wordpress.com/2024/09/16/gobp-goal-oriented-behavior-planning/)

GOBP is an AI system that is designed for complex multi-AI environments where AIs are in a very dynamic environment, requiring sophisticated plans. Rather, plans within plans.

Traditionally GOAP generates a linear plan. A->B->C->D->etc. This is extremely useful in many cases, but for example, in a football game, a linear plan is not sophisticated enough for an NPCs AI.
Behaviiour Trees provide excellent premade plans for NPCs where they can decide "Dribble to this position, check if there's a clear shot, if not, if not much time left still take a shot, otherwise pass it to another player"
However, creating dozens of premade plans is difficult and forces designers to think through every possible scenario for every possible position such Center Forward, Right Wing, Defending Middlefielder etc.
This is where GOBP comes in. It functions pretty much similar to GOAP in the sense of, it takes goals, generates plans for every goals. However, instead of picking the lowest cost, it merges the plans into a one big plan, 
and construct a Behaviour Tree that satisfies every goal, prioritizing actions that have higher goal priority and/or lower cost. This lets the AI have Plan A, B, C at the same time, instead of one single plan.
This allows a more sophisticated behavior as well as reducing the amount of planning needed.

Going back to a linear plan in for example a football (soccer) situation, if it needed to shoot but couldn't it would have to replan. This situation would arise constantly, resulting in constant replanning.
With this approach, the AI does not need to replan for longer period of time, and would have a plan that last for the duration for the attack for example, rather than for a 'moment.'

Cons:
- Due to its complex nature, it cannot be applied to (or at least, it would be an overkill) for games that have more static environments or, less AIs that interact. In football for example, there are 22 AIs (or one player + 21 AI)
- Higher performance cost, due multi-plan generation and BT translation layer.
