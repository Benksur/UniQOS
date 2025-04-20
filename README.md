# UniQOS

## Planned Task Mapping
| Task                   | Priority |
| :--------------------- | -------: |
| Cellular Task          |   High   |
| Audio Task             |   High   |
| Call State Task        |  Medium  |
| Power Management       |  Medium  |
| Watchdog Task          |  Medium  |
| UI/Display Task        | Low-mid  |
| Input handler Task     | Low-mid  |
| Logging Task           |   Low    |

## Task Communication
- Queue from Input → UI
- Queue from UI → Call State
- Queue or event group from Call State → Cellular Task
- Queue from Cellular Task → Call State
- Event group or direct-to-task notification from Call State → Audio Codec Task