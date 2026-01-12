# Supported version

This mod supported [Qounters++](https://github.com/Metalit/Qounters/tree/main).

The qounters feature was enabled at the following version.

Game Version | HeartBeatQuest Version | Qounters version
--- | --- | ---
1.40.8 | >= 0.3.8 | N/A

# Mod Compatability

The qounters++ feature was automatically enabled when supported qounters++ mod is detected.

When qounters++ detected, the bundled UI, which is what you see without qounters++, will be disabled automatically.

# Qounters support

You should add the heart rate manually at qounters' configuration. The following sources are supported.

## Text Source

A text source called `HeartRate` is added to qounters++. It shows your heart rate number with an optional heart icon as prefix/postfix.

## Shape Source

A float number source called `HeartRatePercent` is added to qounters++.

The float value is calculated by your configured max heart rate. This is useful when you want to create some shape that indicates your heart rate zone.

By default, it was aligned as the following value.
HeartRate / MaxHeartRate | source value
--- | ---
`<50%`| 0
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
