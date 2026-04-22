# Supported version

This mod supports [Qounters++](https://github.com/Metalit/Qounters/tree/main).

The qounters feature was enabled at the following version.

Game Version | HeartBeatQuest Version | Qounters version | Comments
--- | --- | --- | ---
`1.40.8` | `>=TBD` | N/A
`<1.40.8`| N/A | N/A | Qounters++ with API isn't avaliable in old games

# Mod Compatability

The qounters++ feature was automatically enabled when supported qounters++ mod is detected.

When qounters++ detected, the bundled UI, which is what you see without qounters++, will be disabled automatically.

# Qounters++ support

You should add the heart rate manually at qounters' configuration. The following sources are supported.

## Text Source

A text source called `HeartRate` is added to qounters++. It shows your heart rate number with an optional heart icon as prefix/postfix.

## Shape Source

A float number source called `HeartRatePercent` is added to qounters++.

The float value is calculated by your configured max heart rate. This is useful when you want to create some shape that indicates your heart rate zone.

By default, it output a value that was aligned as the following value.
HeartRate / MaxHeartRate | aligned output value
--- | ---
`<50%`| 0%
`50%-60%`|20%
`60%-70%`|40%
`70%-80%`|60%
`80%-90%`|80%
`>90%`|100%

However you could configure it to output the `HeartRate / MaxHeartRate` value directly.

## Enable Source

A boolean source called `HeartRatePercentRange` is added to qounters++.

You can configure it to output true or false at each heart rate zone. This is useful when you want to display something as one or more specific heart rate zone.

## Color Source

A color source called `HeartRateRangeColor` is added to qounters++.

You can configure it to output different colors at each heart rate zone. This is useful when you want to change some UI elements' color as the heart rate zone.

# Manual Compat

If you disable qounters++ in the game menu, the HeartRate mod will not load UI in the game. You should manually enable the `Ignore Qounters++ mod` option in the HeartRate setthing menu, then this mod will works.

Do NOT enable that option when you're using qounters.
