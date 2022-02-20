# Entity

Основное представление объекта модели внутри гейммода.

------------------

## Описание основных параметров класса

### Свойства:  
| Название       | Описание |
| ------------- | ---------|
| `int entid` | Идентификатор объкта который использует данную модель. |
| `ModelMeta mdlmeta` | Содержит дополнительную информацию о модели. |
| `ArrayList bglist` | Список активных bodygroups. |

### Методы:  
| Название       | Описание |
| ------------- | ---------|
| `Mdl SetPath(char[] modelName)` | Устанавливает путь к модели. |
| `Mdl GetPath(char[] modelName, int max_size)` | Получает путь к модели. |
| `void SetById(char[] modelid)` | Устанавливает и конфигурирует модель по идентификатору из основго конфига мода |
| `void SetBodyGroup(char[] name, int idx)` | Установка группы тела у модели ("base","head","eyes","helmet","mask","rank","body","belt","legs","pl","pr"). |
| `void SetSkin(int skin)` | Устанавливает скин. |
| `int GetSkin()` | Получает идентификатор скина |
| `Mdl SetRenderMode(RenderMode mode)` | Устанавливает RenderMod (см. флаги SM). |
| `Mdl SetRenderColor(Colour clr)` | Устанавливает RenderColor. |
| `void Dispose()` | Деструктор объекта. |