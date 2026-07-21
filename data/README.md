# Conversation generation - `conv.yml`
## What is important to mention
### What purpose should the message serve
- Keep track of a 'purpose' from a list of predefined purposes that change on each option press (e.g. 'comfort' or 'encourage')
- There are predefined 'initial' and 'reset' purpose lists where a purpose gets randomly selected when the conversation starts/is reset.

### What is there to mention
- Context tags - a list of context items about the user (e.g. 'sad', 'tired', 'unsure', 'had a long day', 'thinking about project xyz', 'bored') that should be 'interesting', i.e. make an impact on messages sent, so include enough and make them specific so each new option will always change at least something and the conversation doesn't go in circles.
  - Context tags get mostly reset at the beginning of every conversation (some groups are marked as 'keep' (prefixed with =; the = gets stripped during parsing) (such as projects working on) and they stay but the rest reset)
  - Context tags should have groups (e.g. 'mood' contains 'sad' and 'happy') where conflicting tags (e.g. 'happy' and 'sad') should be in the same group and there is only ever at most one tag active per group.
  - There are some context tags that are auto generated, and only the group should be used to refer to them. These will be auto-kept whether or not they are prefixed with = because they are not dependent on the conversation (although some are dependent on other context, in which case they will be updated).
  - Context tags must be unique and are referred to by name and not by group

## Realisation (generating the output)
### The sentence
- Purposes have a list of templates grouped based on context tags present
  - The rules are a ", " separated list, and all the requirements must match. They can contain:
    - tag - must have that tag
    - !tag - must not have that tag
    - +group - must have a tag from the group
    - -group - matches when no tag from that group is active
    - = - matches when no other sentence has matched yet (matches are evaluated first-last)
    - rule?rule... - can match any of the provided rules
  - A rule of `*` by itself means match everything
  - Only applicable templates are chosen from (e.g. if one group doesn't have the active tag it isn't avaliable)
  - Each template can also define synonyms (e.g. "I hope you {feel/get} better") or use context groups (e.g. %mood or %time)
    - But adding a context group to a template also adds the requirement that that group must have a tag active to that template
  - Starting a sentence with `x*` (optional space after) where x is a number 'repeats' the sentence x times, making it x times more likely to appear.
- One avaliable template is chosen at random from all avaliable in the current purpose
- If a sentence is in the format `> purpose` then it will set the purpose to the one specified and use a sentence from that instead.

### The options
- The purpose templates are grouped together by the list of user options provided
  - This list contains multiple choices for what set of options to provide and it picks a random one
  - Each option in the set is a word or phrase and has attached tags and an optional set purpose which if included sets the purpose to that.
  - 'Attached tags' are where if the option is selected, the tags are added to the current context, removing others in the same group. Additionally, if a tag is `-group`, it will remove all tags from that group instead.
- A 'new topic' option is always included (no need to specify) that resets the conversation
- If 'shuffle' is true then will shuffle the options. If false or omitted, will keep in order.

### The dictionary
- In most string values (not keys) '$word' can be used to reference something from the dictionary, which will be substituted in-place.

# Format example
I do not recommend to use the contents of this example, it only exists to demonstrate good formatting and that is it, not sample contents.
```yaml
groups:
  mood: [sad, okay, happy]
  time: "time_of_day"

initial: [checkup_mood]
reset: [checkup_mood, checkup_progress]

purposes:
  checkup_mood:
    - templates:
        "sad":
          - "2* I hope you $feel better soon."
          - "That sounds really tough."
        "=, +mood":
          - "You seem to be doing well!"
        "-mood":
          - "How are you doing today?"
        "*":
          - "2* How's your %time{ going/}?"
      shuffle: true
      opts:
        - ["A bit sad", [sad], comfort]
        - ["Pretty good", [okay]]
        - ["Don't know", [-mood]]

dictionary:
  feel: "{feel/start feeling}"
```

## Auto generated context groups
These are used as strings instead of lists of tags under a group name.
- `"time_of_day"` - e.g. "morning", "afternoon", or "evening"
- `"this_time_of_day"` - e.g. "this morning", "this afternoon", "tonight"


# Scenes - `scenes.yml`
## `scenes:`
This contains all the scenes by name. In each scene is a list of items or conditions with sub-items.

An item is a list; `[name, x, y]`. If the name begins with `~`, the asset will be flipped horizontally.

A condition only displays the item if the nametag exists in the current scene. When prefixed with `^`, it will only display if this is NOT the case.

## `assets:`
These contain first keys to asset files then the names of every asset contained in that file. For example:
```yaml
assets:
  furnature:
    - chair-down
    - chair-up
```
States that the `chair-up` and `chair-down` assets are contained within `furnature.svg` (all of these svgs are located in `assets/BG/`)

## `choices:`
These are a list of choices, each equally likely. A choice contains:
- A `base`, which is the base scene name to use
- `extras`, which are a list of 'extras' (tags to add to the current scene). An extra can be either;
  - A string - which will be always used
  - A list - from which a random item is picked (can prefix with `x*` which will increase the likelihood of that option by `x` times). Can include blank items (in yaml, that is `~`)

# Qualities - `quals.yml`
This file is a mapping from quality name to a list of conversation context tags that influence it and by how much.

Each quality contains a mapping containing a score, and a list of all the quality keys which can give this score. All the scores will be tallied and the quality will be given an "overall score", which is 0 if negative, 1 if zero or 2 if positive. All qualities on a task will have their overall scores summed together and normalised between 0 and 1 for its overall score

For example:
```
tiring:
  2: [motivated, happy]
  -1: [bored]
  -2: [tired, stressed]
```
Remember, this is "should this task, which has this quality, be done now?" so it should value opposite tags to the quality
