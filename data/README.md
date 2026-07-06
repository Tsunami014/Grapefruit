# Idea
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
- Purposes have a list of templates grouped based on context tag (e.g. 'comfort' may contain 'sad' which may have things like "I hope you feel better")
    - `~` is used when there is no tag, `+` or `=` is used for any tag that isn't already used but `+` requires there to be a tag from the group.
    - Only applicable templates are chosen from (e.g. if one group doesn't have the active tag it isn't avaliable)
    - Each template can also define synonyms (e.g. "I hope you {feel/get} better") or use context groups (e.g. %mood or %time)
        - But adding a context group to a template also adds the requirement that that group must have a tag active to that template
    - Starting a sentence with `x*` (optional space after) where x is a number 'repeats' the sentence x times, making it x times more likely to appear.
- One avaliable template is chosen at random from all avaliable in the current purpose
- Sentences can be included in a group `$` to always be avaliable (does not contain context tag requirements). This is not recommended, except maybe for purposes that only occur initially when the context is mostly empty anyway.

### The options
- The purpose templates are grouped together by the list of user options provided
    - This list contains multiple choices for what set of options to provide and it picks a random one
    - Each option in the set is a word or phrase and has attached tags and an optional set purpose which if included sets the purpose to that.
    - 'Attached tags' are where if the option is selected, the tags are added to the current context, removing others in the same group. Additionally, if a tag is `-group`, it will remove all tags from that group instead.
- A 'new topic' option is always included (no need to specify) that resets the conversation (setting the purpose to $)

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
    templates:
      $:
        - "2* How's your $time{ going/}?"
      mood:
        sad:
          - "2* I hope you {feel/start feeling} better soon."
          - "That sounds really tough."
        +:
          - "You seem to be doing well!"
        =:
          - "How are you doing today?"
    options:
      - - ["A bit sad", [sad], comfort]
        - ["Pretty good", [okay]]
        - ["Don't know", [-mood]]
```

# Auto generated context groups
These are used as strings instead of lists of tags under a group name.
- `"time_of_day"` - either "morning", "afternoon", or "evening"
