import typing
if typing.TYPE_CHECKING:
    from tools.CutsceneTypes import Cutscene, Target, TargetType, WaitTypes
else:
    from CutsceneTypes import Cutscene, Target, TargetType, WaitTypes


def cutscene(c: Cutscene):
    c.set_animation(Target(TargetType.SPRITE, 1), "down")
    c.set_interact_action(Target(TargetType.SPRITE, 1), "none")
    c.set_collider_enabled(1, False)

    c.set_animation(Target(TargetType.SPRITE, 7), "rightMove")
    c.move_in_frames(Target(TargetType.SPRITE, 7), 640-425, 0, 180)
    c.wait(WaitTypes.FRAMES, 180)
    c.set_animation(Target(TargetType.SPRITE, 7), "leftIdle")
    c.set_interact_action(Target(TargetType.SPRITE, 7), "cutscene",
                          cutscene_id=11)
    c.set_flag(220, 0)
