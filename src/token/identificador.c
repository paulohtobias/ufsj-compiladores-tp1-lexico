/**
 * PCC - Paulo C Compiler
 *
 * token/identificador.c
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#include <stdio.h>
#include <string.h>
#include "token/palavra-chave.h"
#include "utils.h"
#include "plist.h"

#define emoji_pattern "\\x{203C}\\x{2049}\\x{2122}\\x{2139}\\x{2194}-\\x{2199}\\x{21A9}-\\x{21AA}\\x{231A}-\\x{231B}\\x{2328}\\x{23CF}\\x{23E9}-\\x{23F3}\\x{23F8}-\\x{23FA}\\x{24C2}\\x{25AA}-\\x{25AB}\\x{25B6}\\x{25C0}\\x{25FB}-\\x{25FE}\\x{2600}-\\x{2604}\\x{260E}\\x{2611}\\x{2614}-\\x{2615}\\x{2618}\\x{261D}\\x{2620}\\x{2622}-\\x{2623}\\x{2626}\\x{262A}\\x{262E}-\\x{262F}\\x{2638}-\\x{263A}\\x{2640}\\x{2642}\\x{2648}-\\x{2653}\\x{265F}-\\x{2660}\\x{2663}\\x{2665}-\\x{2666}\\x{2668}\\x{267B}\\x{267E}-\\x{267F}\\x{2692}-\\x{2697}\\x{2699}\\x{269B}-\\x{269C}\\x{26A0}-\\x{26A1}\\x{26AA}-\\x{26AB}\\x{26B0}-\\x{26B1}\\x{26BD}-\\x{26BE}\\x{26C4}-\\x{26C5}\\x{26C8}\\x{26CE}\\x{26CF}\\x{26D1}\\x{26D3}-\\x{26D4}\\x{26E9}-\\x{26EA}\\x{26F0}-\\x{26F5}\\x{26F7}-\\x{26FA}\\x{26FD}\\x{2702}\\x{2705}\\x{2708}-\\x{2709}\\x{270A}-\\x{270B}\\x{270C}-\\x{270D}\\x{270F}\\x{2712}\\x{2714}\\x{2716}\\x{271D}\\x{2721}\\x{2728}\\x{2733}-\\x{2734}\\x{2744}\\x{2747}\\x{274C}\\x{274E}\\x{2753}-\\x{2755}\\x{2757}\\x{2763}-\\x{2764}\\x{2795}-\\x{2797}\\x{27A1}\\x{27B0}\\x{27BF}\\x{2934}-\\x{2935}\\x{2B05}-\\x{2B07}\\x{2B1B}-\\x{2B1C}\\x{2B50}\\x{2B55}\\x{3030}\\x{303D}\\x{3297}\\x{3299}\\x{1F004}\\x{1F0CF}\\x{1F170}-\\x{1F171}\\x{1F17E}\\x{1F17F}\\x{1F18E}\\x{1F191}-\\x{1F19A}\\x{1F1E6}-\\x{1F1FF}\\x{1F201}-\\x{1F202}\\x{1F21A}\\x{1F22F}\\x{1F232}-\\x{1F23A}\\x{1F250}-\\x{1F251}\\x{1F300}-\\x{1F320}\\x{1F321}\\x{1F324}-\\x{1F32C}\\x{1F32D}-\\x{1F32F}\\x{1F330}-\\x{1F335}\\x{1F336}\\x{1F337}-\\x{1F37C}\\x{1F37D}\\x{1F37E}-\\x{1F37F}\\x{1F380}-\\x{1F393}\\x{1F396}-\\x{1F397}\\x{1F399}-\\x{1F39B}\\x{1F39E}-\\x{1F39F}\\x{1F3A0}-\\x{1F3C4}\\x{1F3C5}\\x{1F3C6}-\\x{1F3CA}\\x{1F3CB}-\\x{1F3CE}\\x{1F3CF}-\\x{1F3D3}\\x{1F3D4}-\\x{1F3DF}\\x{1F3E0}-\\x{1F3F0}\\x{1F3F3}-\\x{1F3F5}\\x{1F3F7}\\x{1F3F8}-\\x{1F3FF}\\x{1F400}-\\x{1F43E}\\x{1F43F}\\x{1F440}\\x{1F441}\\x{1F442}-\\x{1F4F7}\\x{1F4F8}\\x{1F4F9}-\\x{1F4FC}\\x{1F4FD}\\x{1F4FF}\\x{1F500}-\\x{1F53D}\\x{1F549}-\\x{1F54A}\\x{1F54B}-\\x{1F54E}\\x{1F550}-\\x{1F567}\\x{1F56F}-\\x{1F570}\\x{1F573}-\\x{1F579}\\x{1F57A}\\x{1F587}\\x{1F58A}-\\x{1F58D}\\x{1F590}\\x{1F595}-\\x{1F596}\\x{1F5A4}\\x{1F5A5}\\x{1F5A8}\\x{1F5B1}-\\x{1F5B2}\\x{1F5BC}\\x{1F5C2}-\\x{1F5C4}\\x{1F5D1}-\\x{1F5D3}\\x{1F5DC}-\\x{1F5DE}\\x{1F5E1}\\x{1F5E3}\\x{1F5E8}\\x{1F5EF}\\x{1F5F3}\\x{1F5FA}\\x{1F5FB}-\\x{1F5FF}\\x{1F600}\\x{1F601}-\\x{1F610}\\x{1F611}\\x{1F612}-\\x{1F614}\\x{1F615}\\x{1F616}\\x{1F617}\\x{1F618}\\x{1F619}\\x{1F61A}\\x{1F61B}\\x{1F61C}-\\x{1F61E}\\x{1F61F}\\x{1F620}-\\x{1F625}\\x{1F626}-\\x{1F627}\\x{1F628}-\\x{1F62B}\\x{1F62C}\\x{1F62D}\\x{1F62E}-\\x{1F62F}\\x{1F630}-\\x{1F633}\\x{1F634}\\x{1F635}-\\x{1F640}\\x{1F641}-\\x{1F642}\\x{1F643}-\\x{1F644}\\x{1F645}-\\x{1F64F}\\x{1F680}-\\x{1F6C5}\\x{1F6CB}-\\x{1F6CF}\\x{1F6D0}\\x{1F6D1}-\\x{1F6D2}\\x{1F6D5}\\x{1F6E0}-\\x{1F6E5}\\x{1F6E9}\\x{1F6EB}-\\x{1F6EC}\\x{1F6F0}\\x{1F6F3}\\x{1F6F4}-\\x{1F6F6}\\x{1F6F7}-\\x{1F6F8}\\x{1F6F9}\\x{1F6FA}\\x{1F7E0}-\\x{1F7EB}\\x{1F90D}-\\x{1F90F}\\x{1F910}-\\x{1F918}\\x{1F919}-\\x{1F91E}\\x{1F91F}\\x{1F920}-\\x{1F927}\\x{1F928}-\\x{1F92F}\\x{1F930}\\x{1F931}-\\x{1F932}\\x{1F933}-\\x{1F93A}\\x{1F93C}-\\x{1F93E}\\x{1F93F}\\x{1F940}-\\x{1F945}\\x{1F947}-\\x{1F94B}\\x{1F94C}\\x{1F94D}-\\x{1F94F}\\x{1F950}-\\x{1F95E}\\x{1F95F}-\\x{1F96B}\\x{1F96C}-\\x{1F970}\\x{1F971}\\x{1F973}-\\x{1F976}\\x{1F97A}\\x{1F97B}\\x{1F97C}-\\x{1F97F}\\x{1F980}-\\x{1F984}\\x{1F985}-\\x{1F991}\\x{1F992}-\\x{1F997}\\x{1F998}-\\x{1F9A2}\\x{1F9A5}-\\x{1F9AA}\\x{1F9AE}-\\x{1F9AF}\\x{1F9B0}-\\x{1F9B9}\\x{1F9BA}-\\x{1F9BF}\\x{1F9C0}\\x{1F9C1}-\\x{1F9C2}\\x{1F9C3}-\\x{1F9CA}\\x{1F9CD}-\\x{1F9CF}\\x{1F9D0}-\\x{1F9E6}\\x{1F9E7}-\\x{1F9FF}\\x{1FA70}-\\x{1FA73}\\x{1FA78}-\\x{1FA7A}\\x{1FA80}-\\x{1FA82}\\x{1FA90}-\\x{1FA95}\\x{231A}-\\x{231B}\\x{23E9}-\\x{23EC}\\x{23F0}\\x{23F3}\\x{25FD}-\\x{25FE}\\x{2614}-\\x{2615}\\x{2648}-\\x{2653}\\x{267F}\\x{2693}\\x{26A1}\\x{26AA}-\\x{26AB}\\x{26BD}-\\x{26BE}\\x{26C4}-\\x{26C5}\\x{26CE}\\x{26D4}\\x{26EA}\\x{26F2}-\\x{26F3}\\x{26F5}\\x{26FA}\\x{26FD}\\x{2705}\\x{270A}-\\x{270B}\\x{2728}\\x{274C}\\x{274E}\\x{2753}-\\x{2755}\\x{2757}\\x{2795}-\\x{2797}\\x{27B0}\\x{27BF}\\x{2B1B}-\\x{2B1C}\\x{2B50}\\x{2B55}\\x{1F004}\\x{1F0CF}\\x{1F18E}\\x{1F191}-\\x{1F19A}\\x{1F1E6}-\\x{1F1FF}\\x{1F201}\\x{1F21A}\\x{1F22F}\\x{1F232}-\\x{1F236}\\x{1F238}-\\x{1F23A}\\x{1F250}-\\x{1F251}\\x{1F300}-\\x{1F320}\\x{1F32D}-\\x{1F32F}\\x{1F330}-\\x{1F335}\\x{1F337}-\\x{1F37C}\\x{1F37E}-\\x{1F37F}\\x{1F380}-\\x{1F393}\\x{1F3A0}-\\x{1F3C4}\\x{1F3C5}\\x{1F3C6}-\\x{1F3CA}\\x{1F3CF}-\\x{1F3D3}\\x{1F3E0}-\\x{1F3F0}\\x{1F3F4}\\x{1F3F8}-\\x{1F3FF}\\x{1F400}-\\x{1F43E}\\x{1F440}\\x{1F442}-\\x{1F4F7}\\x{1F4F8}\\x{1F4F9}-\\x{1F4FC}\\x{1F4FF}\\x{1F500}-\\x{1F53D}\\x{1F54B}-\\x{1F54E}\\x{1F550}-\\x{1F567}\\x{1F57A}\\x{1F595}-\\x{1F596}\\x{1F5A4}\\x{1F5FB}-\\x{1F5FF}\\x{1F600}\\x{1F601}-\\x{1F610}\\x{1F611}\\x{1F612}-\\x{1F614}\\x{1F615}\\x{1F616}\\x{1F617}\\x{1F618}\\x{1F619}\\x{1F61A}\\x{1F61B}\\x{1F61C}-\\x{1F61E}\\x{1F61F}\\x{1F620}-\\x{1F625}\\x{1F626}-\\x{1F627}\\x{1F628}-\\x{1F62B}\\x{1F62C}\\x{1F62D}\\x{1F62E}-\\x{1F62F}\\x{1F630}-\\x{1F633}\\x{1F634}\\x{1F635}-\\x{1F640}\\x{1F641}-\\x{1F642}\\x{1F643}-\\x{1F644}\\x{1F645}-\\x{1F64F}\\x{1F680}-\\x{1F6C5}\\x{1F6CC}\\x{1F6D0}\\x{1F6D1}-\\x{1F6D2}\\x{1F6D5}\\x{1F6EB}-\\x{1F6EC}\\x{1F6F4}-\\x{1F6F6}\\x{1F6F7}-\\x{1F6F8}\\x{1F6F9}\\x{1F6FA}\\x{1F7E0}-\\x{1F7EB}\\x{1F90D}-\\x{1F90F}\\x{1F910}-\\x{1F918}\\x{1F919}-\\x{1F91E}\\x{1F91F}\\x{1F920}-\\x{1F927}\\x{1F928}-\\x{1F92F}\\x{1F930}\\x{1F931}-\\x{1F932}\\x{1F933}-\\x{1F93A}\\x{1F93C}-\\x{1F93E}\\x{1F93F}\\x{1F940}-\\x{1F945}\\x{1F947}-\\x{1F94B}\\x{1F94C}\\x{1F94D}-\\x{1F94F}\\x{1F950}-\\x{1F95E}\\x{1F95F}-\\x{1F96B}\\x{1F96C}-\\x{1F970}\\x{1F971}\\x{1F973}-\\x{1F976}\\x{1F97A}\\x{1F97B}\\x{1F97C}-\\x{1F97F}\\x{1F980}-\\x{1F984}\\x{1F985}-\\x{1F991}\\x{1F992}-\\x{1F997}\\x{1F998}-\\x{1F9A2}\\x{1F9A5}-\\x{1F9AA}\\x{1F9AE}-\\x{1F9AF}\\x{1F9B0}-\\x{1F9B9}\\x{1F9BA}-\\x{1F9BF}\\x{1F9C0}\\x{1F9C1}-\\x{1F9C2}\\x{1F9C3}-\\x{1F9CA}\\x{1F9CD}-\\x{1F9CF}\\x{1F9D0}-\\x{1F9E6}\\x{1F9E7}-\\x{1F9FF}\\x{1FA70}-\\x{1FA73}\\x{1FA78}-\\x{1FA7A}\\x{1FA80}-\\x{1FA82}\\x{1FA90}-\\x{1FA95}\\x{1F3FB}-\\x{1F3FF}\\x{261D}\\x{26F9}\\x{270A}-\\x{270B}\\x{270C}-\\x{270D}\\x{1F385}\\x{1F3C2}-\\x{1F3C4}\\x{1F3C7}\\x{1F3CA}\\x{1F3CB}-\\x{1F3CC}\\x{1F442}-\\x{1F443}\\x{1F446}-\\x{1F450}\\x{1F466}-\\x{1F478}\\x{1F47C}\\x{1F481}-\\x{1F483}\\x{1F485}-\\x{1F487}\\x{1F48F}\\x{1F491}\\x{1F4AA}\\x{1F574}-\\x{1F575}\\x{1F57A}\\x{1F590}\\x{1F595}-\\x{1F596}\\x{1F645}-\\x{1F647}\\x{1F64B}-\\x{1F64F}\\x{1F6A3}\\x{1F6B4}-\\x{1F6B6}\\x{1F6C0}\\x{1F6CC}\\x{1F90F}\\x{1F918}\\x{1F919}-\\x{1F91E}\\x{1F91F}\\x{1F926}\\x{1F930}\\x{1F931}-\\x{1F932}\\x{1F933}-\\x{1F939}\\x{1F93C}-\\x{1F93E}\\x{1F9B5}-\\x{1F9B6}\\x{1F9B8}-\\x{1F9B9}\\x{1F9BB}\\x{1F9CD}-\\x{1F9CF}\\x{1F9D1}-\\x{1F9DD}\\x{200D}\\x{20E3}\\x{FE0F}\\x{1F1E6}-\\x{1F1FF}\\x{1F3FB}-\\x{1F3FF}\\x{1F9B0}-\\x{1F9B3}\\x{E0020}-\\x{E007F}\\x{203C}\\x{2049}\\x{2122}\\x{2139}\\x{2194}-\\x{2199}\\x{21A9}-\\x{21AA}\\x{231A}-\\x{231B}\\x{2328}\\x{2388}\\x{23CF}\\x{23E9}-\\x{23F3}\\x{23F8}-\\x{23FA}\\x{24C2}\\x{25AA}-\\x{25AB}\\x{25B6}\\x{25C0}\\x{25FB}-\\x{25FE}\\x{2600}-\\x{2605}\\x{2607}-\\x{2612}\\x{2614}-\\x{2615}\\x{2616}-\\x{2617}\\x{2618}\\x{2619}\\x{261A}-\\x{266F}\\x{2670}-\\x{2671}\\x{2672}-\\x{267D}\\x{267E}-\\x{267F}\\x{2680}-\\x{2685}\\x{2690}-\\x{2691}\\x{2692}-\\x{269C}\\x{269D}\\x{269E}-\\x{269F}\\x{26A0}-\\x{26A1}\\x{26A2}-\\x{26B1}\\x{26B2}\\x{26B3}-\\x{26BC}\\x{26BD}-\\x{26BF}\\x{26C0}-\\x{26C3}\\x{26C4}-\\x{26CD}\\x{26CE}\\x{26CF}-\\x{26E1}\\x{26E2}\\x{26E3}\\x{26E4}-\\x{26E7}\\x{26E8}-\\x{26FF}\\x{2700}\\x{2701}-\\x{2704}\\x{2705}\\x{2708}-\\x{2709}\\x{270A}-\\x{270B}\\x{270C}-\\x{2712}\\x{2714}\\x{2716}\\x{271D}\\x{2721}\\x{2728}\\x{2733}-\\x{2734}\\x{2744}\\x{2747}\\x{274C}\\x{274E}\\x{2753}-\\x{2755}\\x{2757}\\x{2763}-\\x{2767}\\x{2795}-\\x{2797}\\x{27A1}\\x{27B0}\\x{27BF}\\x{2934}-\\x{2935}\\x{2B05}-\\x{2B07}\\x{2B1B}-\\x{2B1C}\\x{2B50}\\x{2B55}\\x{3030}\\x{303D}\\x{3297}\\x{3299}\\x{1F000}-\\x{1F02B}\\x{1F02C}-\\x{1F02F}\\x{1F030}-\\x{1F093}\\x{1F094}-\\x{1F09F}\\x{1F0A0}-\\x{1F0AE}\\x{1F0AF}-\\x{1F0B0}\\x{1F0B1}-\\x{1F0BE}\\x{1F0BF}\\x{1F0C0}\\x{1F0C1}-\\x{1F0CF}\\x{1F0D0}\\x{1F0D1}-\\x{1F0DF}\\x{1F0E0}-\\x{1F0F5}\\x{1F0F6}-\\x{1F0FF}\\x{1F10D}-\\x{1F10F}\\x{1F12F}\\x{1F16C}\\x{1F16D}-\\x{1F16F}\\x{1F170}-\\x{1F171}\\x{1F17E}\\x{1F17F}\\x{1F18E}\\x{1F191}-\\x{1F19A}\\x{1F1AD}-\\x{1F1E5}\\x{1F201}-\\x{1F202}\\x{1F203}-\\x{1F20F}\\x{1F21A}\\x{1F22F}\\x{1F232}-\\x{1F23A}\\x{1F23C}-\\x{1F23F}\\x{1F249}-\\x{1F24F}\\x{1F250}-\\x{1F251}\\x{1F252}-\\x{1F25F}\\x{1F260}-\\x{1F265}\\x{1F266}-\\x{1F2FF}\\x{1F300}-\\x{1F320}\\x{1F321}-\\x{1F32C}\\x{1F32D}-\\x{1F32F}\\x{1F330}-\\x{1F335}\\x{1F336}\\x{1F337}-\\x{1F37C}\\x{1F37D}\\x{1F37E}-\\x{1F37F}\\x{1F380}-\\x{1F393}\\x{1F394}-\\x{1F39F}\\x{1F3A0}-\\x{1F3C4}\\x{1F3C5}\\x{1F3C6}-\\x{1F3CA}\\x{1F3CB}-\\x{1F3CE}\\x{1F3CF}-\\x{1F3D3}\\x{1F3D4}-\\x{1F3DF}\\x{1F3E0}-\\x{1F3F0}\\x{1F3F1}-\\x{1F3F7}\\x{1F3F8}-\\x{1F3FA}\\x{1F400}-\\x{1F43E}\\x{1F43F}\\x{1F440}\\x{1F441}\\x{1F442}-\\x{1F4F7}\\x{1F4F8}\\x{1F4F9}-\\x{1F4FC}\\x{1F4FD}-\\x{1F4FE}\\x{1F4FF}\\x{1F500}-\\x{1F53D}\\x{1F546}-\\x{1F54A}\\x{1F54B}-\\x{1F54F}\\x{1F550}-\\x{1F567}\\x{1F568}-\\x{1F579}\\x{1F57A}\\x{1F57B}-\\x{1F5A3}\\x{1F5A4}\\x{1F5A5}-\\x{1F5FA}\\x{1F5FB}-\\x{1F5FF}\\x{1F600}\\x{1F601}-\\x{1F610}\\x{1F611}\\x{1F612}-\\x{1F614}\\x{1F615}\\x{1F616}\\x{1F617}\\x{1F618}\\x{1F619}\\x{1F61A}\\x{1F61B}\\x{1F61C}-\\x{1F61E}\\x{1F61F}\\x{1F620}-\\x{1F625}\\x{1F626}-\\x{1F627}\\x{1F628}-\\x{1F62B}\\x{1F62C}\\x{1F62D}\\x{1F62E}-\\x{1F62F}\\x{1F630}-\\x{1F633}\\x{1F634}\\x{1F635}-\\x{1F640}\\x{1F641}-\\x{1F642}\\x{1F643}-\\x{1F644}\\x{1F645}-\\x{1F64F}\\x{1F680}-\\x{1F6C5}\\x{1F6C6}-\\x{1F6CF}\\x{1F6D0}\\x{1F6D1}-\\x{1F6D2}\\x{1F6D3}-\\x{1F6D4}\\x{1F6D5}\\x{1F6D6}-\\x{1F6DF}\\x{1F6E0}-\\x{1F6EC}\\x{1F6ED}-\\x{1F6EF}\\x{1F6F0}-\\x{1F6F3}\\x{1F6F4}-\\x{1F6F6}\\x{1F6F7}-\\x{1F6F8}\\x{1F6F9}\\x{1F6FA}\\x{1F6FB}-\\x{1F6FF}\\x{1F774}-\\x{1F77F}\\x{1F7D5}-\\x{1F7D8}\\x{1F7D9}-\\x{1F7DF}\\x{1F7E0}-\\x{1F7EB}\\x{1F7EC}-\\x{1F7FF}\\x{1F80C}-\\x{1F80F}\\x{1F848}-\\x{1F84F}\\x{1F85A}-\\x{1F85F}\\x{1F888}-\\x{1F88F}\\x{1F8AE}-\\x{1F8FF}\\x{1F90C}\\x{1F90D}-\\x{1F90F}\\x{1F910}-\\x{1F918}\\x{1F919}-\\x{1F91E}\\x{1F91F}\\x{1F920}-\\x{1F927}\\x{1F928}-\\x{1F92F}\\x{1F930}\\x{1F931}-\\x{1F932}\\x{1F933}-\\x{1F93A}\\x{1F93C}-\\x{1F93E}\\x{1F93F}\\x{1F940}-\\x{1F945}\\x{1F947}-\\x{1F94B}\\x{1F94C}\\x{1F94D}-\\x{1F94F}\\x{1F950}-\\x{1F95E}\\x{1F95F}-\\x{1F96B}\\x{1F96C}-\\x{1F970}\\x{1F971}\\x{1F972}\\x{1F973}-\\x{1F976}\\x{1F977}-\\x{1F979}\\x{1F97A}\\x{1F97B}\\x{1F97C}-\\x{1F97F}\\x{1F980}-\\x{1F984}\\x{1F985}-\\x{1F991}\\x{1F992}-\\x{1F997}\\x{1F998}-\\x{1F9A2}\\x{1F9A3}-\\x{1F9A4}\\x{1F9A5}-\\x{1F9AA}\\x{1F9AB}-\\x{1F9AD}\\x{1F9AE}-\\x{1F9AF}\\x{1F9B0}-\\x{1F9B9}\\x{1F9BA}-\\x{1F9BF}\\x{1F9C0}\\x{1F9C1}-\\x{1F9C2}\\x{1F9C3}-\\x{1F9CA}\\x{1F9CB}-\\x{1F9CC}\\x{1F9CD}-\\x{1F9CF}\\x{1F9D0}-\\x{1F9E6}\\x{1F9E7}-\\x{1F9FF}\\x{1FA00}-\\x{1FA53}\\x{1FA54}-\\x{1FA5F}\\x{1FA60}-\\x{1FA6D}\\x{1FA6E}-\\x{1FA6F}\\x{1FA70}-\\x{1FA73}\\x{1FA74}-\\x{1FA77}\\x{1FA78}-\\x{1FA7A}\\x{1FA7B}-\\x{1FA7F}\\x{1FA80}-\\x{1FA82}\\x{1FA83}-\\x{1FA8F}\\x{1FA90}-\\x{1FA95}\\x{1FA96}-\\x{1FFFD}"
#define letra_pattern regex_unicode_letter emoji_pattern

void adicionar(const char *arquivo, const char *lexema, size_t comprimento, int32_t linha, int32_t coluna);

int token_identificador_init(afd_t *afd) {
	int res;

	// Construindo o AFD.
	afd_t afd_id;
	afd_init(&afd_id, 1);

	// Estado inicial. Não pode haver números.
	afd_transicao_t transicoes_0[] = {
		{1, {"[a-zA-Z_$]", "[" letra_pattern "]", NULL}}
	};
	afd_id.estados[0] = afd_criar_estado(transicoes_0, ARR_TAMANHO(transicoes_0), false, NULL);

	// Transições.
	afd_transicao_t transicoes_1[] = {
		{1, {"[a-zA-Z0-9_$]", "[" letra_pattern "\\d]", NULL}}
	};
	plist_append(afd_id.estados, afd_criar_estado(transicoes_1, ARR_TAMANHO(transicoes_1), true, adicionar));

	if ((res = afd_mesclar_automatos(afd, &afd_id)) != AFD_OK) {
		LOG_PCC_ERRO(0, NULL, "%s: Não foi possível iniciar: %s\n", __FUNCTION__, afd_get_erro(res));
		goto fim;
	}

	// Inicializando a tabela de símbolos de identificador.
	plist_create(tabela_simbolos[TK_ID], 1);
	tabela_simbolos[TK_ID][0] = NULL;

fim:
	return res;
}

void adicionar(const char *arquivo, const char *lexema, size_t comprimento, int32_t linha, int32_t coluna) {
	token_t token = token_criar(TK_ID, 0, arquivo, lexema, comprimento, linha, coluna);

	// Busca o lexema na tabela de palavras chave.
	int palavra_chave_index = token_palavra_chave_buscar(&token);
	if (palavra_chave_index >= 0) {
		token.tipo = TK_KW;
		token.subtipo = palavra_chave_index;
		token.subtipo_to_str = token_palavra_chave_subtipo_str;
	}

	token_adicionar(&token);
}
