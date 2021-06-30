#ifndef _IRIDIUM_UNITS_H_INCLUDED_
#define _IRIDIUM_UNITS_H_INCLUDED_

#define IRIDIUM_UNITS_NONE                                  0     // Единиц измерения нет
#define IRIDIUM_UNITS_BOOL                                  1     // Бинарное значение отображаемое как "true" и "false"
#define IRIDIUM_UNITS_YES_NO                                2     // Бинарное значение отображаемое как "Да" и "Нет"
#define IRIDIUM_UNITS_ON_OFF                                3     // Бинарное значение отображаемое как "Включено" и "Выключено"
#define IRIDIUM_UNITS_ENABLE_DISABLE                        4     // Бинарное значение отображаемое как "Разрешено" и "Запрещено"
#define IRIDIUM_UNITS_BIN_NUMBER                            5     // Числовое значение отображаемое как бинарное число
#define IRIDIUM_UNITS_DEC_NUMBER                            6     // Числовое значение отображаемое как десятичное число
#define IRIDIUM_UNITS_FLOAT_NUMBER                          7     // Числовое значение отображаемое как число с плавающей запятой
#define IRIDIUM_UNITS_HEX_NUMBER                            8     // Числовое значение отображаемое как шеснадцатеричное число
#define IRIDIUM_UNITS_BIN_ARRAY                             9     // Массив битовых значений
#define IRIDIUM_UNITS_DEC_ARRAY                             10    // Массив десятичных значений
#define IRIDIUM_UNITS_FLOAT_ARRAY                           11    // Массив значений с плавающей запятой
#define IRIDIUM_UNITS_HEX_ARRAY                             12    // Массив шеснадцатеричных значений
#define IRIDIUM_UNITS_STRING                                13    // Строка в формате UTF8
#define IRIDIUM_UNITS_MAC                                   14    // Мак адрес x:x:x:x:x:x
#define IRIDIUM_UNITS_IPV4                                  15    // IPv4 x.x.x.x
#define IRIDIUM_UNITS_IPV6                                  16    // IPv6 x.x.x.x.x.x.x.x
#define IRIDIUM_UNITS_TIME_STRING                           17    // Строка с временем hh:mm:ss.ms или hh:mm:ss или hh:mm
#define IRIDIUM_UNITS_DATA_STRING                           18    // Строка с датой dd.mm.yyyy или yyyy.mm.dd
#define IRIDIUM_UNITS_DATA_AND_TIME_STRING                  19    // Строка с датой и времен dd.mm.yyyy, hh:mm:ss.ms

///////////////////////////////////////////////////////////////////////////
// Единицы измерения объема информации
///////////////////////////////////////////////////////////////////////////
#define IRIDIUM_UNITS_BITS                                  40    // Количество бит
#define IRIDIUM_UNITS_BYTES                                 41    // Количество байт
#define IRIDIUM_UNITS_KIB                                   42    // Количество кибибайт
#define IRIDIUM_UNITS_MIB                                   43    // Количество мебибайт
#define IRIDIUM_UNITS_GIB                                   44    // Количество гибибайт
#define IRIDIUM_UNITS_TIB                                   45    // Количество тебибайт
#define IRIDIUM_UNITS_PIB                                   46    // Количество пебибайт

///////////////////////////////////////////////////////////////////////////
// Единицы измерения скорости передачи информации
///////////////////////////////////////////////////////////////////////////
#define IRIDIUM_UNITS_BITS_PER_SECOND                       47    // Количество бит в секунду
#define IRIDIUM_UNITS_BYTES_PER_SECOND                      48    // Количество байт в секунду
#define IRIDIUM_UNITS_KIB_PER_SECOND                        49    // Количество кибибайт в секунду
#define IRIDIUM_UNITS_MIB_PER_SECOND                        50    // Количество мебибайт в секунду
#define IRIDIUM_UNITS_GIB_PER_SECOND                        51    // Количество гибибайт в секунду
#define IRIDIUM_UNITS_TIB_PER_SECOND                        52    // Количество тебибайт в секунду
#define IRIDIUM_UNITS_PIB_PER_SECOND                        53    // Количество пебибайт в секунду

///////////////////////////////////////////////////////////////////////////
// Единицы измерения быстродействия
///////////////////////////////////////////////////////////////////////////
#define IRIDIUM_UNITS_MEGAFLOPS                             54    // Количество миллионов операций с плавающей запятой в секунду
#define IRIDIUM_UNITS_GIGAFLOPS                             55    // Количество миллиардов операций с плавающей запятой в секунду
#define IRIDIUM_UNITS_TERAFLOPS                             56    // Количество триллионов операций с плавающей запятой в секунду
#define IRIDIUM_UNITS_PETEFLOPS                             57    // Количество квадрилионов операций с плавающей запятой в секунду

///////////////////////////////////////////////////////////////////////////
// Единицы измерения частей и долей
///////////////////////////////////////////////////////////////////////////
#define IRIDIUM_UNITS_PERCENT                               100   // Проценты или колличество сотых частей
#define IRIDIUM_UNITS_PERMILLE                              101   // Промили или колличество тысячных частей
#define IRIDIUM_UNITS_PER_TEN_THOUSAND                      102   // Количество десятитысячных частей
#define IRIDIUM_UNITS_PARTS_PER_MILLION                     103   // Количество миллионных частей
#define IRIDIUM_UNITS_PARTS_PER_BILLION                     104   // Колличество миллиардных частей
#define IRIDIUM_UNITS_PARTS_PER_TRILLION                    105   // Колличество триллионных частей

///////////////////////////////////////////////////////////////////////////
// Единицы измерения длинны
///////////////////////////////////////////////////////////////////////////
// Системные
#define IRIDIUM_UNITS_MILLIMETER                            200   // Миллиметры
#define IRIDIUM_UNITS_CENTIMETER                            201   // Сантиметры
#define IRIDIUM_UNITS_METER                                 202   // Метры
#define IRIDIUM_UNITS_KILOMETER                             203   // Километры
// Внесистемные
#define IRIDIUM_UNITS_INCHES                                230   // Дюймы (Дюйм равен 0,0254 метра)
#define IRIDIUM_UNITS_FEET                                  231   // Футы (Фут равен 12 дюймам или 0,3048 метра)
#define IRIDIUM_UNITS_YARD                                  232   // Ярды (Ярд равен 3 футам или 0,9144 метра)
#define IRIDIUM_UNITS_MILES                                 233   // Мили (Миля равна 1760 ярдам или 1609 метров)

///////////////////////////////////////////////////////////////////////////
// Единицы измерения площади
///////////////////////////////////////////////////////////////////////////
// Системные
#define IRIDIUM_UNITS_SQUARE_MILLIMETERS                    300   // Квадратные миллиметры
#define IRIDIUM_UNITS_SQUARE_CENTIMETERS                    301   // Квадратные сантиметры
#define IRIDIUM_UNITS_SQUARE_METERS                         302   // Квадратные метры
#define IRIDIUM_UNITS_SQUARE_KILOMETERS                     303   // Квадратные километры
// Внесистемные
#define IRIDIUM_UNITS_SQUARE_INCHES                         330   // Квадратные дюймы
#define IRIDIUM_UNITS_SQUARE_FEET                           331   // Квадратные футы
#define IRIDIUM_UNITS_SQUARE_YARD                           332   // Квадратные ярды
#define IRIDIUM_UNITS_SQUARE_MILES                          333   // Квадратные мили
#define IRIDIUM_UNITS_ARE                                   334   // Ар или сотка (10 х 10 = 100 квадратных метров)
#define IRIDIUM_UNITS_HECTARE                               335   // Гектар (100 х 100 = 10000 квадратных метров)

///////////////////////////////////////////////////////////////////////////
// Единицы измерения объема
///////////////////////////////////////////////////////////////////////////
// Системные
#define IRIDIUM_UNITS_CUBIC_MILLIMETERS                     400   // Кубические миллиметры
#define IRIDIUM_UNITS_CUBIC_CENTIMETERS                     401   // Кубические сантиметры
#define IRIDIUM_UNITS_CUBIC_METERS                          402   // Кубические метры
#define IRIDIUM_UNITS_CUBIC_KILOMETERS                      403   // Кубические километры
// Внесистемные
#define IRIDIUM_UNITS_CUBIC_INCHES                          430   // Кубические дюймы
#define IRIDIUM_UNITS_CUBIC_FEET                            431   // Кубические футы
#define IRIDIUM_UNITS_CUBIC_YARD                            432   // Кубические ярды
#define IRIDIUM_UNITS_CUBIC_MILES                           433   // Кубические мили
#define IRIDIUM_UNITS_LITERS                                434   // Литр (равен кубичесткому децеметру)
#define IRIDIUM_UNITS_IMPERIAL_OUNCE_FLUID                  435   // Английская жидкая унция (28,41306 миллилитра)
#define IRIDIUM_UNITS_US_OUNCE_FLUID                        436   // Американская жидкая унция (29,57353 миллилитра)
#define IRIDIUM_UNITS_US_PINTS                              437   // Американская пинта (0,473176473 литра)
#define IRIDIUM_UNITS_IMPERIAL_PINTS                        438   // Английская пинта (0,56826125 литра)
#define IRIDIUM_UNITS_US_QUART                              439   // Американская кварта (0,946352946 литра)
#define IRIDIUM_UNITS_IMPERIAL_QUART                        440   // Английская кварта (1,1365225 литра)
#define IRIDIUM_UNITS_US_GALLON                             441   // Американский галлон (3,785 литра)
#define IRIDIUM_UNITS_IMPERIAL_GALLON                       442   // Английская галлон (4,54609 литра)
#define IRIDIUM_UNITS_OIL_BARREL                            443   // Нефтяной барель (равен 42 американским галонам или 35 английским галлонам или 159 литров)

///////////////////////////////////////////////////////////////////////////
// Единицы измерения времени
///////////////////////////////////////////////////////////////////////////
#define IRIDIUM_UNITS_MILLISECONDS                          500   // Миллисекунды
#define IRIDIUM_UNITS_HUNDREDTHS_SECONDS                    501   // Сотые доли секунды
#define IRIDIUM_UNITS_SECONDS                               502   // Секунды
#define IRIDIUM_UNITS_MINUTES                               503   // Минуты
#define IRIDIUM_UNITS_HOURS                                 504   // Часы
#define IRIDIUM_UNITS_DAYS                                  505   // Дни
#define IRIDIUM_UNITS_WEEKS                                 506   // Недели
#define IRIDIUM_UNITS_MONTHS                                507   // Месяцы
#define IRIDIUM_UNITS_YEARS                                 508   // Годы

///////////////////////////////////////////////////////////////////////////
// Единицы измерения веса
///////////////////////////////////////////////////////////////////////////
// Системные
#define IRIDIUM_UNITS_MILLIGRAMS                            600   // Миллиграммы
#define IRIDIUM_UNITS_GRAMS                                 601   // Граммы
#define IRIDIUM_UNITS_KILOGRAMS                             602   // Килограммы
#define IRIDIUM_UNITS_TONS                                  603   // Тонны
// Внесистемные
#define IRIDIUM_UNITS_KARAT_MASS                            630   // Карат (Равен 0,2 грамма)
#define IRIDIUM_UNITS_OUNCE_MASS                            631   // Международная эвердьюпойсная унция (Равна 28,349523125 грамм)
#define IRIDIUM_UNITS_TROY_OUNCE_MASS                       632   // Международная тройская унция (Равна 31,1034768 грамм)
#define IRIDIUM_UNITS_POUNDS_MASS                           633   // Фунты (Равен 16 международным эвердьюпойсным унциям или 0,453 кг)

///////////////////////////////////////////////////////////////////////////
// Единицы частоты
///////////////////////////////////////////////////////////////////////////
#define IRIDIUM_UNITS_PER_SECOND                            700   // Количество раз в секунду
#define IRIDIUM_UNITS_PER_MINUTE                            701   // Количество раз в минуту
#define IRIDIUM_UNITS_PER_HOUR                              702   // Количество раз в час
#define IRIDIUM_UNITS_PER_DAY                               703   // Количество раз в день
#define IRIDIUM_UNITS_PER_WEEK                              704   // Количество раз в неделю
#define IRIDIUM_UNITS_PER_MONTH                             705   // Количество раз в месяц
#define IRIDIUM_UNITS_PER_YAER                              706   // Количество раз в год
#define IRIDIUM_UNITS_CYCLES_PER_SECOND                     707   // Количество оборотов в секунду
#define IRIDIUM_UNITS_CYCLES_PER_MINUTE                     708   // Количество оборотов в минуту
#define IRIDIUM_UNITS_CYCLES_PER_HOUR                       709   // Количество оборотов в час
#define IRIDIUM_UNITS_HERTZ                                 710   // Герцы
#define IRIDIUM_UNITS_KILOHERTZ                             711   // Килогерцы
#define IRIDIUM_UNITS_MEGAHERTZ                             712   // Мегагерцы
#define IRIDIUM_UNITS_GIGAHERTZ                             713   // Гигагерцы

///////////////////////////////////////////////////////////////////////////
// Единицы измерения скорости
///////////////////////////////////////////////////////////////////////////
// Системные
#define IRIDIUM_UNITS_METERS_PER_SECOND                     800   // Метров в секунду
// Внесистемные
#define IRIDIUM_UNITS_MILLIMETERS_PER_SECOND                830   // Миллиметров в секунду
#define IRIDIUM_UNITS_MILLIMETERS_PER_MINUTE                831   // Миллиметров в минуту
#define IRIDIUM_UNITS_MILLIMETERS_PER_HOUR                  832   // Миллиметров в час
#define IRIDIUM_UNITS_INCHES_PER_SECOND                     833   // Дюймов в секунду
#define IRIDIUM_UNITS_INCHES_PER_MINUTE                     834   // Дюймов в минуту
#define IRIDIUM_UNITS_INCHES_PER_HOUR                       835   // Дюймов в час
#define IRIDIUM_UNITS_FEET_PER_SECOND                       836   // Футов в секунду
#define IRIDIUM_UNITS_FEET_PER_MINUTE                       837   // Футов в минуту
#define IRIDIUM_UNITS_FEET_PER_HOUR                         838   // Футов в час
#define IRIDIUM_UNITS_YARD_PER_SECOND                       839   // Ярдов в секунду
#define IRIDIUM_UNITS_YARD_PER_MINUTE                       840   // Ярдов в минуту
#define IRIDIUM_UNITS_YARD_PER_HOUR                         841   // Ярдов в час
#define IRIDIUM_UNITS_METERS_PER_MINUTE                     842   // Метров в минуту
#define IRIDIUM_UNITS_METERS_PER_HOUR                       843   // Метров в час
#define IRIDIUM_UNITS_KILOMETERS_PER_SECOND                 844   // Километров в секунду
#define IRIDIUM_UNITS_KILOMETERS_PER_MINUTE                 845   // Километров в минуту
#define IRIDIUM_UNITS_KILOMETERS_PER_HOUR                   846   // Километров в час
#define IRIDIUM_UNITS_MILES_PER_SECOND                      847   // Миль в секунду
#define IRIDIUM_UNITS_MILES_PER_MINUTE                      848   // Миль в минуту
#define IRIDIUM_UNITS_MILES_PER_HOUR                        849   // Миль в час
#define IRIDIUM_UNITS_KNOT                                  850   // Узлы (Узел равен 1,852 километру в час)

///////////////////////////////////////////////////////////////////////////
// Объем в единицу времени
///////////////////////////////////////////////////////////////////////////
// Системные
#define IRIDIUM_UNITS_CUBIC_MILLIMETERS_PER_SECOND          900   // Кубических миллиметров в секунду
#define IRIDIUM_UNITS_CUBIC_MILLIMETERS_PER_MINUTE          901   // Кубических миллиметров в минуту
#define IRIDIUM_UNITS_CUBIC_MILLIMETERS_PER_HOUR            902   // Кубических миллиметров в час
#define IRIDIUM_UNITS_CUBIC_CENTIMETERS_PER_SECOND          903   // Кубических сантиметров в секунду
#define IRIDIUM_UNITS_CUBIC_CENTIMETERS_PER_MINUTE          904   // Кубических сантиметров в минуту
#define IRIDIUM_UNITS_CUBIC_CENTIMETERS_PER_HOUR            905   // Кубических сантиметров в час
#define IRIDIUM_UNITS_CUBIC_METERS_PER_SECOND               906   // Кубических метров в секунду
#define IRIDIUM_UNITS_CUBIC_METERS_PER_MINUTE               907   // Кубических метров в минуту
#define IRIDIUM_UNITS_CUBIC_METERS_PER_HOUR                 908   // Кубических метров в час
#define IRIDIUM_UNITS_CUBIC_KILOMETERS_PER_SECOND           909   // Кубических километров в секунду
#define IRIDIUM_UNITS_CUBIC_KILOMETERS_PER_MINUTE           910   // Кубических километров в минуту
#define IRIDIUM_UNITS_CUBIC_KILOMETERS_PER_HOUR             911   // Кубических километров в час
// Внесистемные
#define IRIDIUM_UNITS_CUBIC_INCHES_PER_SECOND               930   // Кубических дюймов в секунду
#define IRIDIUM_UNITS_CUBIC_INCHES_PER_MINUTE               931   // Кубических дюймов в минуту
#define IRIDIUM_UNITS_CUBIC_INCHES_PER_HOUR                 932   // Кубических дюймов в час
#define IRIDIUM_UNITS_CUBIC_FEET_PER_SECOND                 933   // Кубических футов в секунду
#define IRIDIUM_UNITS_CUBIC_FEET_PER_MINUTE                 934   // Кубических футов в минуту
#define IRIDIUM_UNITS_CUBIC_FEET_PER_HOUR                   935   // Кубических футов в час
#define IRIDIUM_UNITS_CUBIC_YARD_PER_SECOND                 936   // Кубических ярдов в секунду
#define IRIDIUM_UNITS_CUBIC_YARD_PER_MINUTE                 937   // Кубических ярдов в минуту
#define IRIDIUM_UNITS_CUBIC_YARD_PER_HOUR                   938   // Кубических ярдов в час
#define IRIDIUM_UNITS_CUBIC_MILES_PER_SECOND                939   // Кубических миль в секунду
#define IRIDIUM_UNITS_CUBIC_MILES_PER_MINUTE                940   // Кубических миль в минуту
#define IRIDIUM_UNITS_CUBIC_MILES_PER_HOUR                  941   // Кубических миль в час
#define IRIDIUM_UNITS_LITERS_PER_SECOND                     942   // Литров в секунду 
#define IRIDIUM_UNITS_FALSTAFF                              942   // Литров в секунду
#define IRIDIUM_UNITS_LITERS_PER_MINUTE                     943   // Литров в минуту
#define IRIDIUM_UNITS_LITERS_PER_HOUR                       944   // Литров в час
#define IRIDIUM_UNITS_IMPERIAL_OUNCE_FLUID_PER_SECOND       945   // Английских жидких унций в секунду
#define IRIDIUM_UNITS_IMPERIAL_OUNCE_FLUID_PER_MINUTE       946   // Английских жидких унций в минуту
#define IRIDIUM_UNITS_IMPERIAL_OUNCE_FLUID_PER_HOUR         947   // Английских жидких унций в час
#define IRIDIUM_UNITS_US_OUNCE_FLUID_PER_SECOND             948   // Американских жидких унций в секунду
#define IRIDIUM_UNITS_US_OUNCE_FLUID_PER_MINUTE             949   // Американских жидких унций в минуту
#define IRIDIUM_UNITS_US_OUNCE_FLUID_PER_HOUR               950   // Американских жидких унций в час
#define IRIDIUM_UNITS_US_PINTS_PER_SECOND                   951   // Американских пинт в секунду
#define IRIDIUM_UNITS_US_PINTS_PER_MINUTE                   952   // Американских пинт в минуту
#define IRIDIUM_UNITS_US_PINTS_PER_HOUR                     953   // Американских пинт в час
#define IRIDIUM_UNITS_IMPERIAL_PINTS_PER_SECOND             954   // Английских пинт в секунду
#define IRIDIUM_UNITS_IMPERIAL_PINTS_PER_MINUTE             955   // Английских пинт в минуту
#define IRIDIUM_UNITS_IMPERIAL_PINTS_PER_HOUR               956   // Английских пинт в час
#define IRIDIUM_UNITS_US_QUART_PER_SECOND                   957   // Американских кварт в секунду
#define IRIDIUM_UNITS_US_QUART_PER_MINUTE                   958   // Американских кварт в минуту
#define IRIDIUM_UNITS_US_QUART_PER_HOUR                     959   // Американских кварт в час
#define IRIDIUM_UNITS_IMPERIAL_QUART_PER_SECOND             960   // Английских кварт в секунду
#define IRIDIUM_UNITS_IMPERIAL_QUART_PER_MINUTE             961   // Английских кварт в минуту
#define IRIDIUM_UNITS_IMPERIAL_QUART_PER_HOUR               962   // Английских кварт в час
#define IRIDIUM_UNITS_US_GALLON_PER_SECOND                  963   // Американских галлонов в секунду
#define IRIDIUM_UNITS_US_GALLON_PER_MINUTE                  964   // Американских галлонов в минуту
#define IRIDIUM_UNITS_US_GALLON_PER_HOUR                    965   // Американских галлонов в час
#define IRIDIUM_UNITS_IMPERIAL_GALLON_PER_SECOND            966   // Английских галлонов в секунду
#define IRIDIUM_UNITS_IMPERIAL_GALLON_PER_MINUTE            967   // Английских галлонов в минуту
#define IRIDIUM_UNITS_IMPERIAL_GALLON_PER_HOUR              968   // Английских галлонов в час
#define IRIDIUM_UNITS_OIL_BARREL_PER_SECOND                 969   // Нефтянных барелей в секунду
#define IRIDIUM_UNITS_OIL_BARREL_PER_MINUTE                 970   // Нефтянных барелей в минуту
#define IRIDIUM_UNITS_OIL_BARREL_PER_HOUR                   971   // Нефтянных барелей в час

///////////////////////////////////////////////////////////////////////////
// Вес в единицу времени
///////////////////////////////////////////////////////////////////////////
// Системные
#define IRIDIUM_UNITS_MILLIGRAMS_PRE_SECOND                 1000  // Миллиграмм в секунду
#define IRIDIUM_UNITS_MILLIGRAMS_PRE_MINUTE                 1001  // Миллиграмм в минуту
#define IRIDIUM_UNITS_MILLIGRAMS_PRE_HOUR                   1002  // Миллиграмм в час
#define IRIDIUM_UNITS_GRAMS_PRE_SECOND                      1003  // Грамм в секунду
#define IRIDIUM_UNITS_GRAMS_PRE_MINUTE                      1004  // Грамм в минуту
#define IRIDIUM_UNITS_GRAMS_PRE_HOUR                        1005  // Грамм в час
#define IRIDIUM_UNITS_KILOGRAMS_PRE_SECOND                  1006  // Килограмм в секунду
#define IRIDIUM_UNITS_KILOGRAMS_PRE_MINUTE                  1007  // Килограмм в минуту
#define IRIDIUM_UNITS_KILOGRAMS_PRE_HOUR                    1008  // Килограмм в час
#define IRIDIUM_UNITS_TONS_PRE_SECOND                       1009  // Тонн в секунду
#define IRIDIUM_UNITS_TONS_PRE_MINUTE                       1010  // Тонн в минуту
#define IRIDIUM_UNITS_TONS_PRE_HOUR                         1011  // Тонн в час
// Внесистемные
#define IRIDIUM_UNITS_KARAT_MASS_PRE_SECOND                 1030  // Карат в секунду
#define IRIDIUM_UNITS_KARAT_MASS_PRE_MINUTE                 1031  // Карат в минуту
#define IRIDIUM_UNITS_KARAT_MASS_PRE_HOUR                   1032  // Карат в час
#define IRIDIUM_UNITS_OUNCE_MASS_PRE_SECOND                 1032  // Международных эвердьюпойсных унций в секунду
#define IRIDIUM_UNITS_OUNCE_MASS_PRE_MINUTE                 1033  // Международных эвердьюпойсных унций в минуту
#define IRIDIUM_UNITS_OUNCE_MASS_PRE_HOUR                   1034  // Международных эвердьюпойсных унций в час
#define IRIDIUM_UNITS_TROY_OUNCE_MASS_PRE_SECOND            1035  // Международных тройских унций в секунду
#define IRIDIUM_UNITS_TROY_OUNCE_MASS_PRE_MINUTE            1036  // Международных тройских унций в минуту
#define IRIDIUM_UNITS_TROY_OUNCE_MASS_PRE_HOUR              1037  // Международных тройских унций в час
#define IRIDIUM_UNITS_POUNDS_MASS_PRE_SECOND                1038  // Фунтов в секунду
#define IRIDIUM_UNITS_POUNDS_MASS_PRE_MINUTE                1039  // Фунтов в минуту
#define IRIDIUM_UNITS_POUNDS_MASS_PRE_HOUR                  1040  // Фунтов в час

///////////////////////////////////////////////////////////////////////////
// Электрические параметры
///////////////////////////////////////////////////////////////////////////
// Системные
#define IRIDIUM_UNITS_MILLIAMPERES                          1100  // Миллиамперы
#define IRIDIUM_UNITS_AMPERES                               1101  // Амперы
#define IRIDIUM_UNITS_KILOAMPERES                           1102  // Килоамперы
#define IRIDIUM_UNITS_MEGAAMPERES                           1103  // Мегаамперы
#define IRIDIUM_UNITS_MILLIOHMS                             1104  // Миллиомы
#define IRIDIUM_UNITS_OHMS                                  1105  // Сопративление Омы
#define IRIDIUM_UNITS_KILOHMS                               1106  // Килоомы
#define IRIDIUM_UNITS_MEGOHMS                               1107  // Мегаомы
#define IRIDIUM_UNITS_SIEMENS                               1108  // Электрическая проводимость (равна 1 mho)
#define IRIDIUM_UNITS_MILLIVOLTS                            1109  // Милливольты
#define IRIDIUM_UNITS_VOLTS                                 1110  // Вольты
#define IRIDIUM_UNITS_KILOVOLTS                             1111  // Киловольты
#define IRIDIUM_UNITS_MEGAVOLTS                             1112  // Мегавольты
#define IRIDIUM_UNITS_FARADS                                1113  // Единица измерения электрической ёмкости
#define IRIDIUM_UNITS_HENRYS                                1114  // Единица измерения индуктивности
#define IRIDIUM_UNITS_WEBERS                                1115  // Единица измерения магнитного потока
#define IRIDIUM_UNITS_TESLAS                                1116  // Единица индукции магнитного поля
// Внесистемные
#define IRIDIUM_UNITS_AMPERES_PER_METER                     1130  // Ампер на метр
#define IRIDIUM_UNITS_AMPERES_PER_SQUARE_METER              1131  // Ампер на квадратный метр
#define IRIDIUM_UNITS_AMPERE_SQUARE_METERS                  1132  // Магнитный момент
#define IRIDIUM_UNITS_OHM_METERS                            1133  // Ом на метр
#define IRIDIUM_UNITS_SIEMENS_PER_METER                     1134  // Сименс на метр
#define IRIDIUM_UNITS_VOLT_AMPERES                          1135  // Вольт-амперы
#define IRIDIUM_UNITS_KILOVOLT_AMPERES                      1136  // Киловоль-амперы
#define IRIDIUM_UNITS_MEGAVOLT_AMPERES                      1137  // Мегавольт-амперы
#define IRIDIUM_UNITS_VOLT_AMPERES_REACTIVE                 1138  // Вольт-ампер реактивный
#define IRIDIUM_UNITS_KILOVOLT_AMPERES_REACTIVE             1139  // Киловольт-ампер реактивный
#define IRIDIUM_UNITS_MEGAVOLT_AMPERES_REACTIVE             1140  // Мегавольт-ампер реактивный
#define IRIDIUM_UNITS_VOLTS_PER_DEGREE_KELVIN               1141  // Вольт на градус кельвина
#define IRIDIUM_UNITS_VOLTS_PER_METER                       1142  // Вольт на метр
#define IRIDIUM_UNITS_DEGREES_PHASE                         1143  // Градусы фазы
#define IRIDIUM_UNITS_POWER_FACTOR                          1144  // Коэффициент мощности

///////////////////////////////////////////////////////////////////////////
// Температура
///////////////////////////////////////////////////////////////////////////
#define IRIDIUM_UNITS_DEGREES_KELVIN                        1200  // Температура в кельвинах
#define IRIDIUM_UNITS_DEGREES_CELSIUS                       1201  // Температура в цельсих
#define IRIDIUM_UNITS_DEGREES_FAHRENHEIT                    1202  // Температура в ференгейтах
#define IRIDIUM_UNITS_DEGREE_DAYS_KELVIN                    1203  // Градусо-день в кельвинах
#define IRIDIUM_UNITS_DEGREE_DAYS_CELSIUS                   1204  // Градусо-день в цельсиях
#define IRIDIUM_UNITS_DEGREE_DAYS_FAHRENHEIT                1205  // Градусо-день в ференгейтах

///////////////////////////////////////////////////////////////////////////
// Мощность
///////////////////////////////////////////////////////////////////////////
// Системные
#define IRIDIUM_UNITS_MILLIWATTS                            1300  // Милливатт
#define IRIDIUM_UNITS_WATTS                                 1301  // Ватты
#define IRIDIUM_UNITS_KILOWATTS                             1302  // Киловатты
#define IRIDIUM_UNITS_MEGAWATTS                             1303  // Мегаватты
// Внесистемные
#define IRIDIUM_UNITS_METRIC_HORSEPOWER                     1330  // Метрическая лошадиная сила. (Равна 735,5 ватт)
#define IRIDIUM_UNITS_IMPERIAL_HORSEPOWER                   1331  // Механическая или имперская лошадиная сила. (Равна 745,7 ватт)
#define IRIDIUM_UNITS_BTUS_PER_HOUR                         1332  // Британских тепловых единиц в час (1 единица равна приблизительно 0.293 ватта)
#define IRIDIUM_UNITS_KILO_BTUS_PER_HOUR                    1333  // Тысячу британских тепловых единиц в час (Равна приблизительно 293 ваттам)
#define IRIDIUM_UNITS_TONS_REFRIGERATION                    1334  // Тонна охлаждения (Равна приблизительно 12 000 британским темпловым единицам в час или приблизительно 3500 Ватт)

///////////////////////////////////////////////////////////////////////////
// Давление
///////////////////////////////////////////////////////////////////////////
// Системные
#define IRIDIUM_UNITS_PASCALS                               1400  // Паскаль
#define IRIDIUM_UNITS_HECTOPASCALS                          1401  // Сот паскалей
#define IRIDIUM_UNITS_KILOPASCALS                           1402  // Килопаскали
#define IRIDIUM_UNITS_MEGAPASCALS                           1403  // Мегапаскали
// Внесистемные
#define IRIDIUM_UNITS_MILLIBARS                             1430  // Миллибары
#define IRIDIUM_UNITS_BARS                                  1431  // Бар (1 бар равер 100000 паскелей)
#define IRIDIUM_UNITS_TECHNICAL_ATMOSPHERE                  1432  // Техническая атмосфера (Равна 98066,5 паскалем)
#define IRIDIUM_UNITS_STANDARD_ATMOSPHERE                   1433  // Стандартная атмосфера (Равна 101325 паскалем)
#define IRIDIUM_UNITS_MILLIMETERS_OF_MERCURY                1434  // Миллиметров ртутного столба
#define IRIDIUM_UNITS_INCHES_OF_MERCURY                     1435  // Дюймов ртутного столба
#define IRIDIUM_UNITS_MILLIMETERS_OF_WATER                  1436  // Миллиметров водяного стролба
#define IRIDIUM_UNITS_INCHES_OF_WATER                       1437  // Дюймов водяного столба
#define IRIDIUM_UNITS_POUNDS_FORCE_PER_SQUARE_INCH          1438  // Фунт на квадратный дюйм

///////////////////////////////////////////////////////////////////////////
// Энергия
///////////////////////////////////////////////////////////////////////////
#define IRIDIUM_UNITS_JOULES                                1500  // Джоуль (1 ватт в секунду)
#define IRIDIUM_UNITS_KILOJOULES                            1501  // Килоджоуль
#define IRIDIUM_UNITS_MEGAJOULES                            1502  // Мегаджоуль
#define IRIDIUM_UNITS_WATT_HOURS                            1503  // Ватт в час
#define IRIDIUM_UNITS_KILOWATT_HOURS                        1504  // Киловатт в час
#define IRIDIUM_UNITS_MEGAWATT_HOURS                        1505  // Мегаватт в час
#define IRIDIUM_UNITS_BTUS                                  1506  // Британская тепловая единица (Равна приблизительно 1055,87 джоулей, равна энергии, необходимой для нагревания 1 фунта воды на 1 градус Фаренгейта)
#define IRIDIUM_UNITS_KILO_BTUS                             1507  // Тысячв британских тепловых единиц
#define IRIDIUM_UNITS_MEGA_BTUS                             1508  // Миллион британских тепловых единиц
#define IRIDIUM_UNITS_THERMS                                1509  // Терм (Равна 100000 британским тепловым единицам или 105506000 джоулей, соответствует количеству энергии от сжигания 100 кубических футов природного газа)

///////////////////////////////////////////////////////////////////////////
// Освещенность
///////////////////////////////////////////////////////////////////////////
#define IRIDIUM_UNITS_CANDELAS                              1600  // Сила света
#define IRIDIUM_UNITS_CANDELAS_PER_SQUARE_METER             1601  // Сила света на квадратный метр
#define IRIDIUM_UNITS_WATTS_PER_SQUARE_FOOT                 1602  // Ватт на квадратный фут
#define IRIDIUM_UNITS_WATTS_PER_SQUARE_METER                1603  // Ватт на квадратный метр
#define IRIDIUM_UNITS_LUMENS                                1604  // Световой поток
#define IRIDIUM_UNITS_LUXES                                 1605  // Освещенность
#define IRIDIUM_UNITS_FOOT_CANDLES                          1606  // Сила света на фут

///////////////////////////////////////////////////////////////////////////
// Прочее
///////////////////////////////////////////////////////////////////////////
#define IRIDIUM_UNITS_UNITS_METERS_PER_SECOND_PER_SECOND    1700  // Ускорение
#define IRIDIUM_UNITS_JOULES_PER_KILOGRAM_DRY_AIR           1701  // Количество джоулей на килограмм сухого воздуха
#define IRIDIUM_UNITS_KILOJOULES_PER_KILOGRAM_DRY_AIR       1702  // Количество килоджоулей на килограмм сухого воздуха
#define IRIDIUM_UNITS_MEGAJOULES_PER_KILOGRAM_DRY_AIR       1703  // Количество мегаджоулей на килограмм сухого воздуха
#define IRIDIUM_UNITS_BTUS_PER_POUND_DRY_AIR                1704  // Количество британских тепловых единиц на фунт сухого воздуха
#define IRIDIUM_UNITS_BTUS_PER_POUND                        1705  // Количество британских тепловых единиц на фунт
#define IRIDIUM_UNITS_KILOJOULES_PER_KILOGRAM               1706  // Количество энергии в килождоулях на килограмм
#define IRIDIUM_UNITS_JOULES_PER_DEGREE                     1707  // Количество джоулей для нагрева на градус цельсия/кельвина
#define IRIDIUM_UNITS_KILOJOULES_PER_DEGREE                 1708  // Количество килоджоулей для нагрева на градус цельсия/кельвина
#define IRIDIUM_UNITS_MEGAJOULES_PER_DEGREE                 1709  // Количество мегаджоулей для нагрева на градус цельсия/кельвина
#define IRIDIUM_UNITS_JOULES_PER_KILOGRAM_DEGREE            1710  // Количество мегаджоулей для нагрева килограмма на градус цельсия/кельвина
#define IRIDIUM_UNITS_NEWTON                                1711  // Ньютоны
#define IRIDIUM_UNITS_GRAMS_OF_WATER_PER_KILOGRAM_DRY_AIR   1712  // Количество грамм воды на килограмм сухого воздуха
#define IRIDIUM_UNITS_PERCENT_RELATIVE_HUMIDITY             1713  // Процент влажности
#define IRIDIUM_UNITS_DEGREES_ANGULAR                       1714  // Угловые градусы
#define IRIDIUM_UNITS_DEGREES_CELSIUS_PER_HOUR              1715  // Градусы цельсия в час
#define IRIDIUM_UNITS_DEGREES_CELSIUS_PER_MINUTE            1716  // Градусы цельсия в минуту
#define IRIDIUM_UNITS_DEGREES_FAHRENHEIT_PER_HOUR           1717  // Градусы фарингейта в час
#define IRIDIUM_UNITS_DEGREES_FAHRENHEIT_PER_MINUTE         1718  // Градусы фарингейта в минуту
#define IRIDIUM_UNITS_JOULE_SECONDS                         1719  // Джоуль-секунда
#define IRIDIUM_UNITS_KILOGRAMS_PER_CUBIC_METER             1720  // Плотность
#define IRIDIUM_UNITS_KW_HOURS_PER_SQUARE_METER             1721  // Киловатт час на квадратный метр
#define IRIDIUM_UNITS_KW_HOURS_PER_SQUARE_FOOT              1722  // Киловатт час на квадратный фут
#define IRIDIUM_UNITS_MEGAJOULES_PER_SQUARE_METER           1723  // Мегаджоулей на квадратный метр
#define IRIDIUM_UNITS_MEGAJOULES_PER_SQUARE_FOOT            1724  // Мегаджоулей на квадратный фут
#define IRIDIUM_UNITS_NEWTON_SECONDS                        1725  // Ньютоновская секунда
#define IRIDIUM_UNITS_NEWTONS_PER_METER                     1726  // Ньютон на метр
#define IRIDIUM_UNITS_PERCENT_OBSCURATION_PER_FOOT          1727  // Процент затемнения на фут
#define IRIDIUM_UNITS_PERCENT_OBSCURATION_PER_METER         1728  // Процент затемнения на метр
#define IRIDIUM_UNITS_PERCENT_PER_SECOND                    1729  // Процент в секунду
#define IRIDIUM_UNITS_PSI_PER_DEGREE_FAHRENHEIT             1730  // psi на градус Фаренгейта
#define IRIDIUM_UNITS_RADIANS                               1731  // Радианы
#define IRIDIUM_UNITS_RADIANS_PER_SECOND                    1732  // Радианы в секунду
#define IRIDIUM_UNITS_REVOLUTIONS_PER_MINUTE                1733  // Оборотов в минуту
#define IRIDIUM_UNITS_SQUARE_METERS_PER_NEWTON              1734  // Квадратных метров на Ньютон
#define IRIDIUM_UNITS_WATTS_PER_METER_PER_DEGREE            1735  // Ватт на метра на градус
#define IRIDIUM_UNITS_WATTS_PER_SQUARE_METER_DEGREE         1736  // Ватт на квадратный метра на градус

#endif   // _IRIDIUM_UNITS_H_INCLUDED_
