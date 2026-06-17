## Data Structures And Algorithms Group Assignments 

# Project Planned: ---

> [!IMPORTANT]
> **Use ```make windows```, ```make linux``` to compile the code**

### Rules

- No built-in headers (all functions related to DSA must be made manually)
- Pipelines;
  1. Code Implementation
  2. Video Demonstration (Not too long)
  3. Digital Portfolio/Report
  4. The Payload (Compress source code to .zip format, including video link, portfolio)
- Must include Front Page and Evaluation Form
- Rename ALL your files according to this format: **G\<Group\>_S\<Section\>_BERR**
- Presentation and demonstration will be done in **Week 14 & 15**
 
### Markings

| Aspects | Marks |
| --- | --- | 
| Ideation | 5 |
| Creativity & Design | 5 |
| Overall Presentation & Clarity | 5 |
| Quality | 10 |
| Functionality | 10 |
| Video | 10 |
| Coding Implementation | 15 |

<hr>

### To-Do List

#### Implementation
Atleast 4 of these:
- [x] Array (Grid, enemy postion, dialogue and more)
- [x] Queue (Dialogue Animation; Boss attack plan in Boss.cpp)
- [x] Stack (Boss "rage charge" combo system in Boss.cpp)
- [x] Linked-List (Inventory System)
- [x] Tree (Boss decision tree / AI brain in Boss.cpp)
- [ ] Sorting
- [x] Searching (BFS for enemy detection)
#### Post Implementation
##### Team Portfolio
- [ ] Video (Documentation, Reflections)
- [ ] Front Page, Intro, Objectives & Ideation
- [ ] Methodology, Implementation, Results
- [ ] Challenges and Solution, Conclusion

##### Report
- [ ] File(Include Evaluation Form)
- [ ] Rename and Zip

_please add more tasks_

<hr>

### Library/Program Used

- raylib
- C++

<hr>

### Testing / Debug Room

A self-contained test room is included for the demo/marking.

- In the spawn level, open the chest in the open hall (left side) to get the **Master Key**, then step on the portal beside it.
- The debug room (`src/levels/debug.txt`) contains every item in chests, all enemy types (Slime, Skeleton, and the Boss that uses Queue/Stack/Tree), a key-gated alcove (open with the Iron Key found inside), a signpost, and a return portal.
- Debug hotkeys (overworld): **B** = jump straight into the Boss fight, **N** = jump into a normal fight. Both are repeatable.

<hr>

### Idea

- Turn-Based Game (Similar to Pokemon) + Path Finding Game (Undertale?)
- Similar battle structure as pokemon (linked-list)
- Path finding (searching)
- Pixel art graphics
- Worldbuilding
