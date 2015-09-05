//#line 1 "/usr/home/ben/projects/kanji/c/build/minidic-fixed-text.c.tmpl"
#include "minidic-fixed-text.h"
const char * minidic_kanji_menu =
"\n"
"<table id=\"kanji_menu\">\n"
"<tr>\n"
"\n"
"\n"
"<td onclick='go_to (\"codes.html\");' class=\"page_tab not_chosen_tab\"><a href=\"codes.html\">Input codes\n"
"</a>\n"
"</td>\n"
"\n"
"<td onclick='go_to (\"mr.html\");' class=\"page_tab not_chosen_tab\"><a href=\"mr.html\">Multiradical\n"
"</a>\n"
"</td>\n"
"\n"
"<td onclick='go_to (\"draw.html\");' class=\"page_tab not_chosen_tab\"><a href=\"draw.html\">Handwritten\n"
"</a>\n"
"</td>\n"
"\n"
"<td onclick='go_to (\"four.html\");' class=\"page_tab not_chosen_tab\"><a href=\"four.html\">Four-corner\n"
"</a>\n"
"</td>\n"
"\n"
"<td onclick='go_to (\"soft-keyboard.html\");' class=\"page_tab not_chosen_tab\"><a href=\"soft-keyboard.html\">Kana input\n"
"</a>\n"
"</td>\n"
"</tr><tr>\n"
"\n"
"\n"
"<td onclick='go_to (\"radicals.html\");' class=\"page_tab not_chosen_tab\"><a href=\"radicals.html\">Radicals\n"
"</a>\n"
"</td>\n"
"\n"
"<td onclick='go_to (\"mr-old.html\");' class=\"page_tab not_chosen_tab\"><a href=\"mr-old.html\">Multiradical (old)\n"
"</a>\n"
"</td>\n"
"\n"
"<td onclick='go_to (\"draw-canvas.html\");' class=\"page_tab not_chosen_tab\"><a href=\"draw-canvas.html\">Handwritten (canvas)\n"
"</a>\n"
"</td>\n"
"\n"
"<td onclick='go_to (\"skip.html\");' class=\"page_tab not_chosen_tab\"><a href=\"skip.html\">SKIP\n"
"</a>\n"
"</td>\n"
"\n"
"<td onclick='go_to (\"option.html\");' class=\"page_tab not_chosen_tab\"><a href=\"option.html\">Options\n"
"</a>\n"
"</td>\n"
"</tr>\n"
"</table>\n"
"\n"
;
const char * minidic_html_bottom =
"<script type=\"text/javascript\">\n"
"var gaJsHost = ((\"https:\" == document.location.protocol) ? \"https://ssl.\" : \"http://www.\");\n"
"document.write(unescape(\"%3Cscript src='\" + gaJsHost + \"google-analytics.com/ga.js' type='text/javascript'%3E%3C/script%3E\"));\n"
"</script>\n"
"<script type=\"text/javascript\">\n"
"try {\n"
"var pageTracker = _gat._getTracker(\"UA-11731688-2\");\n"
"pageTracker._trackPageview();\n"
"} catch(err) {}</script>\n"
"\n"
"<script>\n"
"var ml = new Object ();ml.empty_search=\"There is nothing to search for.\";</script>\n"
"</body>\n"
"</html>\n"
;
const char * minidic_html_top =
"<!DOCTYPE HTML>\n"
"<html lang=\"en\">\n"
"<head>\n"
"<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">\n"
"<title>%s kanji information</title>\n"
"\n"
"<link rel=stylesheet href=\"kanji-30.css\" media=all>\n"
"<link rel=stylesheet href=\"minidic-30.css\" media=all>\n"
"<script type=\"text/javascript\" src=\"kanji-30.js\"></script>\n"
"<script type=\"text/javascript\">\n"
"window.google_analytics_uacct = \"UA-11731688-2\";\n"
"</script>\n"
"</head>\n"
"<body  onload=\"initialize_input_box (); \">\n"
;
const char * minidic_message =
"";
const char * minidic_contact =
"<p id=\"contact-info\">\n"
"For queries, please\n"
"email <a href=\"mailto:benkasminbullock@gmail.com\">Ben Bullock</a>\n"
"or use the\n"
"<a href=\"http://groups.google.com/group/sljfaqorg\">discussion forum</a>.\n"
"</p>";
//#line 7 "/usr/home/ben/projects/kanji/c/build/minidic-fixed-text.c.tmpl"


//#line 25 "/usr/home/ben/projects/kanji/c/build/minidic-fixed-text.c.tmpl"

const char * minidic_kanji_code_table =
"\n"
"<table class='kanji-codes'>\n"
"\n"
"<tr>\n"
"<td>\n"
"<a href='http://www.sljfaq.org/afaq/encodings.html#encodings-UTF-16'>\n"
"Unicode</a>\n"
"</td>\n"
"<td>%4X</td>\n"
"<tr>\n"
"<td>\n"
"<a href='http://www.sljfaq.org/afaq/encodings.html#encodings-JIS-X-0208'>\n"
"JIS</a>\n"
"</td>\n"
"<td>%4X</td>\n"
"<tr>\n"
"<td>\n"
"<a href='http://www.sljfaq.org/afaq/encodings.html#encodings-EUC-JP'>\n"
"EUC</a>\n"
"</td>\n"
"<td>%4X</td>\n"
"<tr>\n"
"<td>\n"
"<a href='http://www.sljfaq.org/afaq/encodings.html#encodings-JIS-X-0208'>\n"
"Kuten</a>\n"
"</td>\n"
"<td>%02d-%02d</td>\n"
"<tr>\n"
"<td>\n"
"<a href='http://www.sljfaq.org/afaq/encodings.html#encodings-Shift-JIS'>\n"
"Shift JIS</a>\n"
"</td>\n"
"<td>%4X</td>\n"
"</table>";
//#line 29 "/usr/home/ben/projects/kanji/c/build/minidic-fixed-text.c.tmpl"


//#line 47 "/usr/home/ben/projects/kanji/c/build/minidic-fixed-text.c.tmpl"

const char * minidic_table_headings =
"\n"
"<tr>\n"
"<th>\n"
"Kanji\n"
"</th>\n"
"<th>\n"
"Stroke order\n"
"</th>\n"
"<th>\n"
"Lookup information\n"
"</th>\n"
"<th>\n"
"Computer codes\n"
"</th>\n"
"</tr>\n"
;
//#line 51 "/usr/home/ben/projects/kanji/c/build/minidic-fixed-text.c.tmpl"

#ifdef HEADER

typedef struct radical_variant_name {
    const char * name;
    const char * romaji;
}
radical_variant_name_t;

typedef struct radical_variant_info {
    const char * number;
    const char * character;
    int strokes;
    const char type;
    int unicode;
    int unicode_radical;
}
radical_variant_info_t;

typedef struct radical_info {
    const char * unicode_name;
    const char * number;
    radical_variant_info_t * variants;
}
radical_info_t;

#endif /* def HEADER */


radical_variant_info_t radvar_1[] = {
{
    .number = "1",
    .character = "一",
},
};

radical_variant_info_t radvar_2[] = {
{
    .number = "2",
    .character = "丨",
},
};

radical_variant_info_t radvar_3[] = {
{
    .number = "3",
    .character = "丶",
},
};

radical_variant_info_t radvar_4[] = {
{
    .number = "4",
    .character = "丿",
},
};

radical_variant_info_t radvar_5[] = {
{
    .number = "5.1",
    .character = "乙",
},
{
    .number = "5.2",
    .character = "乚",
},
};

radical_variant_info_t radvar_6[] = {
{
    .number = "6",
    .character = "亅",
},
};

radical_variant_info_t radvar_7[] = {
{
    .number = "7",
    .character = "二",
},
};

radical_variant_info_t radvar_8[] = {
{
    .number = "8",
    .character = "亠",
},
};

radical_variant_info_t radvar_9[] = {
{
    .number = "9.1",
    .character = "人",
},
{
    .number = "9.2",
    .character = "亻",
},
{
    .number = "9.3",
    .character = "",
},
};

radical_variant_info_t radvar_10[] = {
{
    .number = "10",
    .character = "儿",
},
};

radical_variant_info_t radvar_11[] = {
{
    .number = "11",
    .character = "入",
},
};

radical_variant_info_t radvar_12[] = {
{
    .number = "12",
    .character = "八",
},
};

radical_variant_info_t radvar_13[] = {
{
    .number = "13",
    .character = "冂",
},
};

radical_variant_info_t radvar_14[] = {
{
    .number = "14",
    .character = "冖",
},
};

radical_variant_info_t radvar_15[] = {
{
    .number = "15",
    .character = "冫",
},
};

radical_variant_info_t radvar_16[] = {
{
    .number = "16.1",
    .character = "几",
},
{
    .number = "16.2",
    .character = "⺇",
},
};

radical_variant_info_t radvar_17[] = {
{
    .number = "17",
    .character = "凵",
},
};

radical_variant_info_t radvar_18[] = {
{
    .number = "18.1",
    .character = "刀",
},
{
    .number = "18.2",
    .character = "刂",
},
};

radical_variant_info_t radvar_19[] = {
{
    .number = "19",
    .character = "力",
},
};

radical_variant_info_t radvar_20[] = {
{
    .number = "20",
    .character = "勹",
},
};

radical_variant_info_t radvar_21[] = {
{
    .number = "21",
    .character = "匕",
},
};

radical_variant_info_t radvar_22[] = {
{
    .number = "22",
    .character = "匚",
},
};

radical_variant_info_t radvar_23[] = {
{
    .number = "23",
    .character = "匸",
},
};

radical_variant_info_t radvar_24[] = {
{
    .number = "24",
    .character = "十",
},
};

radical_variant_info_t radvar_25[] = {
{
    .number = "25",
    .character = "卜",
},
};

radical_variant_info_t radvar_26[] = {
{
    .number = "26.1",
    .character = "卩",
},
{
    .number = "26.2",
    .character = "",
},
};

radical_variant_info_t radvar_27[] = {
{
    .number = "27",
    .character = "厂",
},
};

radical_variant_info_t radvar_28[] = {
{
    .number = "28",
    .character = "厶",
},
};

radical_variant_info_t radvar_29[] = {
{
    .number = "29.1",
    .character = "又",
},
{
    .number = "29.2",
    .character = "",
},
};

radical_variant_info_t radvar_30[] = {
{
    .number = "30",
    .character = "口",
},
};

radical_variant_info_t radvar_31[] = {
{
    .number = "31",
    .character = "囗",
},
};

radical_variant_info_t radvar_32[] = {
{
    .number = "32.1",
    .character = "土",
},
{
    .number = "32.2",
    .character = "",
},
};

radical_variant_info_t radvar_33[] = {
{
    .number = "33",
    .character = "士",
},
};

radical_variant_info_t radvar_34[] = {
{
    .number = "34.1",
    .character = "夂",
},
{
    .number = "34.2",
    .character = "⼡",
},
};

radical_variant_info_t radvar_35[] = {
{
    .number = "35",
    .character = "夊",
},
};

radical_variant_info_t radvar_36[] = {
{
    .number = "36",
    .character = "夕",
},
};

radical_variant_info_t radvar_37[] = {
{
    .number = "37",
    .character = "大",
},
};

radical_variant_info_t radvar_38[] = {
{
    .number = "38.1",
    .character = "女",
},
{
    .number = "38.2",
    .character = "",
},
};

radical_variant_info_t radvar_39[] = {
{
    .number = "39.1",
    .character = "子",
},
{
    .number = "39.2",
    .character = "",
},
};

radical_variant_info_t radvar_40[] = {
{
    .number = "40",
    .character = "宀",
},
};

radical_variant_info_t radvar_41[] = {
{
    .number = "41",
    .character = "寸",
},
};

radical_variant_info_t radvar_42[] = {
{
    .number = "42.1",
    .character = "小",
},
{
    .number = "42.2",
    .character = "⺌",
},
};

radical_variant_info_t radvar_43[] = {
{
    .number = "43",
    .character = "尢",
},
};

radical_variant_info_t radvar_44[] = {
{
    .number = "44",
    .character = "尸",
},
};

radical_variant_info_t radvar_45[] = {
{
    .number = "45",
    .character = "屮",
},
};

radical_variant_info_t radvar_46[] = {
{
    .number = "46.1",
    .character = "山",
},
{
    .number = "46.2",
    .character = "",
},
{
    .number = "46.3",
    .character = "",
},
};

radical_variant_info_t radvar_47[] = {
{
    .number = "47.1",
    .character = "巛",
},
{
    .number = "47.2",
    .character = "川",
},
};

radical_variant_info_t radvar_48[] = {
{
    .number = "48.1",
    .character = "工",
},
{
    .number = "48.2",
    .character = "",
},
};

radical_variant_info_t radvar_49[] = {
{
    .number = "49.1",
    .character = "己",
},
{
    .number = "49.2",
    .character = "巳",
},
};

radical_variant_info_t radvar_50[] = {
{
    .number = "50",
    .character = "巾",
},
};

radical_variant_info_t radvar_51[] = {
{
    .number = "51",
    .character = "干",
},
};

radical_variant_info_t radvar_52[] = {
{
    .number = "52",
    .character = "幺",
},
};

radical_variant_info_t radvar_53[] = {
{
    .number = "53",
    .character = "广",
},
};

radical_variant_info_t radvar_54[] = {
{
    .number = "54",
    .character = "廴",
},
};

radical_variant_info_t radvar_55[] = {
{
    .number = "55",
    .character = "廾",
},
};

radical_variant_info_t radvar_56[] = {
{
    .number = "56",
    .character = "弋",
},
};

radical_variant_info_t radvar_57[] = {
{
    .number = "57.1",
    .character = "弓",
},
{
    .number = "57.2",
    .character = "",
},
};

radical_variant_info_t radvar_58[] = {
{
    .number = "58.1",
    .character = "彐",
},
{
    .number = "58.2",
    .character = "",
},
{
    .number = "58.3",
    .character = "彑",
},
};

radical_variant_info_t radvar_59[] = {
{
    .number = "59",
    .character = "彡",
},
};

radical_variant_info_t radvar_60[] = {
{
    .number = "60",
    .character = "彳",
},
};

radical_variant_info_t radvar_61[] = {
{
    .number = "61.1",
    .character = "心",
},
{
    .number = "61.2",
    .character = "忄",
},
{
    .number = "61.3",
    .character = "",
},
};

radical_variant_info_t radvar_62[] = {
{
    .number = "62",
    .character = "戈",
},
};

radical_variant_info_t radvar_63[] = {
{
    .number = "63.1",
    .character = "戶",
},
{
    .number = "63.2",
    .character = "戸",
},
};

radical_variant_info_t radvar_64[] = {
{
    .number = "64.1",
    .character = "手",
},
{
    .number = "64.2",
    .character = "扌",
},
};

radical_variant_info_t radvar_65[] = {
{
    .number = "65.1",
    .character = "支",
},
{
    .number = "65.2",
    .character = "⺙",
},
};

radical_variant_info_t radvar_66[] = {
{
    .number = "66.1",
    .character = "攴",
},
{
    .number = "66.2",
    .character = "攵",
},
};

radical_variant_info_t radvar_67[] = {
{
    .number = "67.1",
    .character = "文",
},
{
    .number = "67.2",
    .character = "",
},
};

radical_variant_info_t radvar_68[] = {
{
    .number = "68",
    .character = "斗",
},
};

radical_variant_info_t radvar_69[] = {
{
    .number = "69",
    .character = "斤",
},
};

radical_variant_info_t radvar_70[] = {
{
    .number = "70",
    .character = "方",
},
};

radical_variant_info_t radvar_71[] = {
{
    .number = "71.1",
    .character = "无",
},
{
    .number = "71.2",
    .character = "旡",
},
};

radical_variant_info_t radvar_72[] = {
{
    .number = "72.1",
    .character = "日",
},
{
    .number = "72.2",
    .character = "",
},
};

radical_variant_info_t radvar_73[] = {
{
    .number = "73.1",
    .character = "曰",
},
{
    .number = "73.2",
    .character = "",
},
};

radical_variant_info_t radvar_74[] = {
{
    .number = "74.1",
    .character = "月",
},
{
    .number = "74.2",
    .character = "",
},
{
    .number = "74.3",
    .character = "",
},
};

radical_variant_info_t radvar_75[] = {
{
    .number = "75.1",
    .character = "木",
},
{
    .number = "75.2",
    .character = "",
},
};

radical_variant_info_t radvar_76[] = {
{
    .number = "76",
    .character = "欠",
},
};

radical_variant_info_t radvar_77[] = {
{
    .number = "77.1",
    .character = "止",
},
{
    .number = "77.2",
    .character = "",
},
};

radical_variant_info_t radvar_78[] = {
{
    .number = "78",
    .character = "歹",
},
};

radical_variant_info_t radvar_79[] = {
{
    .number = "79",
    .character = "殳",
},
};

radical_variant_info_t radvar_80[] = {
{
    .number = "80.1",
    .character = "毋",
},
{
    .number = "80.2",
    .character = "毌",
},
{
    .number = "80.3",
    .character = "母",
},
};

radical_variant_info_t radvar_81[] = {
{
    .number = "81",
    .character = "比",
},
};

radical_variant_info_t radvar_82[] = {
{
    .number = "82",
    .character = "毛",
},
};

radical_variant_info_t radvar_83[] = {
{
    .number = "83",
    .character = "氏",
},
};

radical_variant_info_t radvar_84[] = {
{
    .number = "84",
    .character = "气",
},
};

radical_variant_info_t radvar_85[] = {
{
    .number = "85.1",
    .character = "水",
},
{
    .number = "85.2",
    .character = "氵",
},
{
    .number = "85.3",
    .character = "氺",
},
};

radical_variant_info_t radvar_86[] = {
{
    .number = "86.1",
    .character = "火",
},
{
    .number = "86.2",
    .character = "",
},
{
    .number = "86.3",
    .character = "灬",
},
};

radical_variant_info_t radvar_87[] = {
{
    .number = "87.1",
    .character = "爪",
},
{
    .number = "87.2",
    .character = "",
},
{
    .number = "87.3",
    .character = "爫",
},
};

radical_variant_info_t radvar_88[] = {
{
    .number = "88.1",
    .character = "父",
},
};

radical_variant_info_t radvar_89[] = {
{
    .number = "89.1",
    .character = "爻",
},
{
    .number = "89.2",
    .character = "",
},
};

radical_variant_info_t radvar_90[] = {
{
    .number = "90",
    .character = "爿",
},
};

radical_variant_info_t radvar_91[] = {
{
    .number = "91",
    .character = "片",
},
};

radical_variant_info_t radvar_92[] = {
{
    .number = "92",
    .character = "牙",
},
};

radical_variant_info_t radvar_93[] = {
{
    .number = "93.1",
    .character = "牛",
},
{
    .number = "93.2",
    .character = "牜",
},
};

radical_variant_info_t radvar_94[] = {
{
    .number = "94.1",
    .character = "犬",
},
{
    .number = "94.2",
    .character = "犭",
},
};

radical_variant_info_t radvar_95[] = {
{
    .number = "95",
    .character = "玄",
},
};

radical_variant_info_t radvar_96[] = {
{
    .number = "96.1",
    .character = "玉",
},
{
    .number = "96.2",
    .character = "",
},
};

radical_variant_info_t radvar_97[] = {
{
    .number = "97",
    .character = "瓜",
},
};

radical_variant_info_t radvar_98[] = {
{
    .number = "98",
    .character = "瓦",
},
};

radical_variant_info_t radvar_99[] = {
{
    .number = "99",
    .character = "甘",
},
};

radical_variant_info_t radvar_100[] = {
{
    .number = "100",
    .character = "生",
},
};

radical_variant_info_t radvar_101[] = {
{
    .number = "101",
    .character = "用",
},
};

radical_variant_info_t radvar_102[] = {
{
    .number = "102",
    .character = "田",
},
};

radical_variant_info_t radvar_103[] = {
{
    .number = "103.1",
    .character = "疋",
},
{
    .number = "103.2",
    .character = "",
},
};

radical_variant_info_t radvar_104[] = {
{
    .number = "104",
    .character = "疒",
},
};

radical_variant_info_t radvar_105[] = {
{
    .number = "105",
    .character = "癶",
},
};

radical_variant_info_t radvar_106[] = {
{
    .number = "106",
    .character = "白",
},
};

radical_variant_info_t radvar_107[] = {
{
    .number = "107",
    .character = "皮",
},
};

radical_variant_info_t radvar_108[] = {
{
    .number = "108",
    .character = "皿",
},
};

radical_variant_info_t radvar_109[] = {
{
    .number = "109",
    .character = "目",
},
};

radical_variant_info_t radvar_110[] = {
{
    .number = "110",
    .character = "矛",
},
};

radical_variant_info_t radvar_111[] = {
{
    .number = "111.1",
    .character = "矢",
},
{
    .number = "111.2",
    .character = "",
},
};

radical_variant_info_t radvar_112[] = {
{
    .number = "112.1",
    .character = "石",
},
{
    .number = "112.2",
    .character = "",
},
};

radical_variant_info_t radvar_113[] = {
{
    .number = "113.1",
    .character = "示",
},
{
    .number = "113.2",
    .character = "礻",
},
{
    .number = "113.3",
    .character = "",
},
};

radical_variant_info_t radvar_114[] = {
{
    .number = "114",
    .character = "禸",
},
};

radical_variant_info_t radvar_115[] = {
{
    .number = "115.1",
    .character = "禾",
},
{
    .number = "115.2",
    .character = "",
},
};

radical_variant_info_t radvar_116[] = {
{
    .number = "116.1",
    .character = "穴",
},
{
    .number = "116.2",
    .character = "",
},
};

radical_variant_info_t radvar_117[] = {
{
    .number = "117.1",
    .character = "立",
},
{
    .number = "117.2",
    .character = "",
},
};

radical_variant_info_t radvar_118[] = {
{
    .number = "118.1",
    .character = "竹",
},
{
    .number = "118.2",
    .character = "⺮",
},
};

radical_variant_info_t radvar_119[] = {
{
    .number = "119.1",
    .character = "米",
},
{
    .number = "119.2",
    .character = "",
},
};

radical_variant_info_t radvar_120[] = {
{
    .number = "120.1",
    .character = "糸",
},
{
    .number = "120.2",
    .character = "糹",
},
};

radical_variant_info_t radvar_121[] = {
{
    .number = "121",
    .character = "缶",
},
};

radical_variant_info_t radvar_122[] = {
{
    .number = "122.1",
    .character = "网",
},
{
    .number = "122.2",
    .character = "罒",
},
{
    .number = "122.3",
    .character = "罓",
},
};

radical_variant_info_t radvar_123[] = {
{
    .number = "123.1",
    .character = "羊",
},
{
    .number = "123.2",
    .character = "",
},
{
    .number = "123.3",
    .character = "",
},
};

radical_variant_info_t radvar_124[] = {
{
    .number = "124.1",
    .character = "羽",
},
{
    .number = "124.2",
    .character = "",
},
};

radical_variant_info_t radvar_125[] = {
{
    .number = "125.1",
    .character = "老",
},
{
    .number = "125.2",
    .character = "耂",
},
};

radical_variant_info_t radvar_126[] = {
{
    .number = "126",
    .character = "而",
},
};

radical_variant_info_t radvar_127[] = {
{
    .number = "127.1",
    .character = "耒",
},
{
    .number = "127.2",
    .character = "",
},
{
    .number = "127.3",
    .character = "",
},
};

radical_variant_info_t radvar_128[] = {
{
    .number = "128.1",
    .character = "耳",
},
{
    .number = "128.2",
    .character = "",
},
};

radical_variant_info_t radvar_129[] = {
{
    .number = "129",
    .character = "聿",
},
};

radical_variant_info_t radvar_130[] = {
{
    .number = "130.1",
    .character = "肉",
},
{
    .number = "130.2",
    .character = "",
},
{
    .number = "130.3",
    .character = "",
},
};

radical_variant_info_t radvar_131[] = {
{
    .number = "131",
    .character = "臣",
},
};

radical_variant_info_t radvar_132[] = {
{
    .number = "132",
    .character = "自",
},
};

radical_variant_info_t radvar_133[] = {
{
    .number = "133",
    .character = "至",
},
};

radical_variant_info_t radvar_134[] = {
{
    .number = "134",
    .character = "臼",
},
};

radical_variant_info_t radvar_135[] = {
{
    .number = "135.1",
    .character = "舌",
},
{
    .number = "135.2",
    .character = "",
},
};

radical_variant_info_t radvar_136[] = {
{
    .number = "136.1",
    .character = "舛",
},
{
    .number = "136.2",
    .character = "",
},
};

radical_variant_info_t radvar_137[] = {
{
    .number = "137.1",
    .character = "舟",
},
{
    .number = "137.2",
    .character = "",
},
};

radical_variant_info_t radvar_138[] = {
{
    .number = "138",
    .character = "艮",
},
};

radical_variant_info_t radvar_139[] = {
{
    .number = "139.1",
    .character = "色",
},
{
    .number = "139.2",
    .character = "",
},
};

radical_variant_info_t radvar_140[] = {
{
    .number = "140.1",
    .character = "艸",
},
{
    .number = "140.2",
    .character = "艹",
},
{
    .number = "140.3",
    .character = "⺿",
},
};

radical_variant_info_t radvar_141[] = {
{
    .number = "141",
    .character = "虍",
},
};

radical_variant_info_t radvar_142[] = {
{
    .number = "142.1",
    .character = "虫",
},
{
    .number = "142.2",
    .character = "",
},
};

radical_variant_info_t radvar_143[] = {
{
    .number = "143.1",
    .character = "血",
},
{
    .number = "143.2",
    .character = "",
},
};

radical_variant_info_t radvar_144[] = {
{
    .number = "144.1",
    .character = "行",
},
{
    .number = "144.2",
    .character = "",
},
};

radical_variant_info_t radvar_145[] = {
{
    .number = "145.1",
    .character = "衣",
},
{
    .number = "145.2",
    .character = "衤",
},
{
    .number = "145.3",
    .character = "",
},
};

radical_variant_info_t radvar_146[] = {
{
    .number = "146.1",
    .character = "西",
},
{
    .number = "146.2",
    .character = "襾",
},
{
    .number = "146.3",
    .character = "覀",
},
};

radical_variant_info_t radvar_147[] = {
{
    .number = "147",
    .character = "見",
},
};

radical_variant_info_t radvar_148[] = {
{
    .number = "148.1",
    .character = "角",
},
{
    .number = "148.2",
    .character = "",
},
};

radical_variant_info_t radvar_149[] = {
{
    .number = "149.1",
    .character = "言",
},
{
    .number = "149.2",
    .character = "",
},
};

radical_variant_info_t radvar_150[] = {
{
    .number = "150.1",
    .character = "谷",
},
{
    .number = "150.2",
    .character = "",
},
};

radical_variant_info_t radvar_151[] = {
{
    .number = "151.1",
    .character = "豆",
},
{
    .number = "151.2",
    .character = "",
},
};

radical_variant_info_t radvar_152[] = {
{
    .number = "152.1",
    .character = "豕",
},
{
    .number = "152.2",
    .character = "",
},
};

radical_variant_info_t radvar_153[] = {
{
    .number = "153",
    .character = "豸",
},
};

radical_variant_info_t radvar_154[] = {
{
    .number = "154.1",
    .character = "貝",
},
{
    .number = "154.2",
    .character = "",
},
};

radical_variant_info_t radvar_155[] = {
{
    .number = "155.1",
    .character = "赤",
},
{
    .number = "155.2",
    .character = "",
},
};

radical_variant_info_t radvar_156[] = {
{
    .number = "156.1",
    .character = "走",
},
{
    .number = "156.2",
    .character = "",
},
};

radical_variant_info_t radvar_157[] = {
{
    .number = "157.1",
    .character = "足",
},
{
    .number = "157.2",
    .character = "",
},
};

radical_variant_info_t radvar_158[] = {
{
    .number = "158.1",
    .character = "身",
},
{
    .number = "158.2",
    .character = "",
},
};

radical_variant_info_t radvar_159[] = {
{
    .number = "159.1",
    .character = "車",
},
{
    .number = "159.2",
    .character = "",
},
};

radical_variant_info_t radvar_160[] = {
{
    .number = "160.1",
    .character = "辛",
},
{
    .number = "160.2",
    .character = "",
},
};

radical_variant_info_t radvar_161[] = {
{
    .number = "161",
    .character = "辰",
},
};

radical_variant_info_t radvar_162[] = {
{
    .number = "162.1",
    .character = "辵",
},
{
    .number = "162.2",
    .character = "辶",
},
{
    .number = "162.3",
    .character = "⻍",
},
};

radical_variant_info_t radvar_163[] = {
{
    .number = "163.1",
    .character = "邑",
},
{
    .number = "163.2",
    .character = "阝",
},
};

radical_variant_info_t radvar_164[] = {
{
    .number = "164.1",
    .character = "酉",
},
{
    .number = "164.2",
    .character = "",
},
};

radical_variant_info_t radvar_165[] = {
{
    .number = "165.1",
    .character = "釆",
},
{
    .number = "165.2",
    .character = "",
},
};

radical_variant_info_t radvar_166[] = {
{
    .number = "166.1",
    .character = "里",
},
{
    .number = "166.2",
    .character = "",
},
};

radical_variant_info_t radvar_167[] = {
{
    .number = "167.1",
    .character = "金",
},
{
    .number = "167.2",
    .character = "",
},
};

radical_variant_info_t radvar_168[] = {
{
    .number = "168.1",
    .character = "長",
},
{
    .number = "168.2",
    .character = "镸",
},
};

radical_variant_info_t radvar_169[] = {
{
    .number = "169",
    .character = "門",
},
};

radical_variant_info_t radvar_170[] = {
{
    .number = "170.1",
    .character = "阜",
},
{
    .number = "170.2",
    .character = "阝",
},
};

radical_variant_info_t radvar_171[] = {
{
    .number = "171",
    .character = "隶",
},
};

radical_variant_info_t radvar_172[] = {
{
    .number = "172",
    .character = "隹",
},
};

radical_variant_info_t radvar_173[] = {
{
    .number = "173.1",
    .character = "雨",
},
{
    .number = "173.2",
    .character = "⻗",
},
};

radical_variant_info_t radvar_174[] = {
{
    .number = "174.1",
    .character = "靑",
},
{
    .number = "174.2",
    .character = "青",
},
};

radical_variant_info_t radvar_175[] = {
{
    .number = "175",
    .character = "非",
},
};

radical_variant_info_t radvar_176[] = {
{
    .number = "176",
    .character = "面",
},
};

radical_variant_info_t radvar_177[] = {
{
    .number = "177.1",
    .character = "革",
},
{
    .number = "177.2",
    .character = "",
},
};

radical_variant_info_t radvar_178[] = {
{
    .number = "178",
    .character = "韋",
},
};

radical_variant_info_t radvar_179[] = {
{
    .number = "179",
    .character = "韭",
},
};

radical_variant_info_t radvar_180[] = {
{
    .number = "180.1",
    .character = "音",
},
{
    .number = "180.2",
    .character = "",
},
};

radical_variant_info_t radvar_181[] = {
{
    .number = "181",
    .character = "頁",
},
};

radical_variant_info_t radvar_182[] = {
{
    .number = "182.1",
    .character = "風",
},
{
    .number = "182.2",
    .character = "",
},
};

radical_variant_info_t radvar_183[] = {
{
    .number = "183",
    .character = "飛",
},
};

radical_variant_info_t radvar_184[] = {
{
    .number = "184.1",
    .character = "食",
},
{
    .number = "184.2",
    .character = "飠",
},
{
    .number = "184.3",
    .character = "",
},
};

radical_variant_info_t radvar_185[] = {
{
    .number = "185",
    .character = "首",
},
};

radical_variant_info_t radvar_186[] = {
{
    .number = "186",
    .character = "香",
},
};

radical_variant_info_t radvar_187[] = {
{
    .number = "187.1",
    .character = "馬",
},
{
    .number = "187.2",
    .character = "",
},
};

radical_variant_info_t radvar_188[] = {
{
    .number = "188.1",
    .character = "骨",
},
{
    .number = "188.2",
    .character = "",
},
};

radical_variant_info_t radvar_189[] = {
{
    .number = "189",
    .character = "高",
},
};

radical_variant_info_t radvar_190[] = {
{
    .number = "190",
    .character = "髟",
},
};

radical_variant_info_t radvar_191[] = {
{
    .number = "191",
    .character = "鬥",
},
};

radical_variant_info_t radvar_192[] = {
{
    .number = "192",
    .character = "鬯",
},
};

radical_variant_info_t radvar_193[] = {
{
    .number = "193.1",
    .character = "鬲",
},
{
    .number = "193.2",
    .character = "",
},
};

radical_variant_info_t radvar_194[] = {
{
    .number = "194.1",
    .character = "鬼",
},
{
    .number = "194.2",
    .character = "",
},
};

radical_variant_info_t radvar_195[] = {
{
    .number = "195.1",
    .character = "魚",
},
{
    .number = "195.2",
    .character = "",
},
};

radical_variant_info_t radvar_196[] = {
{
    .number = "196.1",
    .character = "鳥",
},
{
    .number = "196.2",
    .character = "",
},
};

radical_variant_info_t radvar_197[] = {
{
    .number = "197",
    .character = "鹵",
},
};

radical_variant_info_t radvar_198[] = {
{
    .number = "198",
    .character = "鹿",
},
};

radical_variant_info_t radvar_199[] = {
{
    .number = "199.1",
    .character = "麥",
},
{
    .number = "199.2",
    .character = "麦",
},
{
    .number = "199.3",
    .character = "",
},
{
    .number = "199.4",
    .character = "",
},
};

radical_variant_info_t radvar_200[] = {
{
    .number = "200.1",
    .character = "麻",
},
{
    .number = "200.2",
    .character = "",
},
{
    .number = "200.3",
    .character = "",
},
};

radical_variant_info_t radvar_201[] = {
{
    .number = "201.1",
    .character = "黄",
},
{
    .number = "201.2",
    .character = "黃",
},
};

radical_variant_info_t radvar_202[] = {
{
    .number = "202",
    .character = "黍",
},
};

radical_variant_info_t radvar_203[] = {
{
    .number = "203.1",
    .character = "黑",
},
{
    .number = "203.2",
    .character = "黒",
},
};

radical_variant_info_t radvar_204[] = {
{
    .number = "204",
    .character = "黹",
},
};

radical_variant_info_t radvar_205[] = {
{
    .number = "205",
    .character = "黽",
},
};

radical_variant_info_t radvar_206[] = {
{
    .number = "206",
    .character = "鼎",
},
};

radical_variant_info_t radvar_207[] = {
{
    .number = "207",
    .character = "鼓",
},
};

radical_variant_info_t radvar_208[] = {
{
    .number = "208",
    .character = "鼠",
},
};

radical_variant_info_t radvar_209[] = {
{
    .number = "209.1",
    .character = "鼻",
},
{
    .number = "209.2",
    .character = "",
},
{
    .number = "209.3",
    .character = "",
},
};

radical_variant_info_t radvar_210[] = {
{
    .number = "210.1",
    .character = "齊",
},
{
    .number = "210.2",
    .character = "斉",
},
};

radical_variant_info_t radvar_211[] = {
{
    .number = "211.1",
    .character = "齒",
},
{
    .number = "211.2",
    .character = "",
},
{
    .number = "211.4",
    .character = "歯",
},
};

radical_variant_info_t radvar_212[] = {
{
    .number = "212.1",
    .character = "龍",
},
{
    .number = "212.2",
    .character = "竜",
},
};

radical_variant_info_t radvar_213[] = {
{
    .number = "213.1",
    .character = "龜",
},
{
    .number = "213.2",
    .character = "亀",
},
};

radical_variant_info_t radvar_214[] = {
{
    .number = "214",
    .character = "龠",
},
};



{
    .number = "1",
},



{
    .number = "2",
},



{
    .number = "3",
},



{
    .number = "4",
},



{
    .number = "5.1",
},

{
    .number = "5.2",
},



{
    .number = "6",
},



{
    .number = "7",
},



{
    .number = "8",
},



{
    .number = "9.1",
},

{
    .number = "9.2",
},

{
    .number = "9.3",
},



{
    .number = "10",
},



{
    .number = "11",
},



{
    .number = "12",
},



{
    .number = "13",
},



{
    .number = "14",
},



{
    .number = "15",
},



{
    .number = "16.1",
},

{
    .number = "16.2",
},



{
    .number = "17",
},



{
    .number = "18.1",
},

{
    .number = "18.2",
},



{
    .number = "19",
},



{
    .number = "20",
},



{
    .number = "21",
},



{
    .number = "22",
},



{
    .number = "23",
},



{
    .number = "24",
},



{
    .number = "25",
},



{
    .number = "26.1",
},

{
    .number = "26.2",
},



{
    .number = "27",
},



{
    .number = "28",
},



{
    .number = "29.1",
},

{
    .number = "29.2",
},



{
    .number = "30",
},



{
    .number = "31",
},



{
    .number = "32.1",
},

{
    .number = "32.2",
},



{
    .number = "33",
},



{
    .number = "34.1",
},

{
    .number = "34.2",
},



{
    .number = "35",
},



{
    .number = "36",
},



{
    .number = "37",
},



{
    .number = "38.1",
},

{
    .number = "38.2",
},



{
    .number = "39.1",
},

{
    .number = "39.2",
},



{
    .number = "40",
},



{
    .number = "41",
},



{
    .number = "42.1",
},

{
    .number = "42.2",
},



{
    .number = "43",
},



{
    .number = "44",
},



{
    .number = "45",
},



{
    .number = "46.1",
},

{
    .number = "46.2",
},

{
    .number = "46.3",
},



{
    .number = "47.1",
},

{
    .number = "47.2",
},



{
    .number = "48.1",
},

{
    .number = "48.2",
},



{
    .number = "49.1",
},

{
    .number = "49.2",
},



{
    .number = "50",
},



{
    .number = "51",
},



{
    .number = "52",
},



{
    .number = "53",
},



{
    .number = "54",
},



{
    .number = "55",
},



{
    .number = "56",
},



{
    .number = "57.1",
},

{
    .number = "57.2",
},



{
    .number = "58.1",
},

{
    .number = "58.2",
},

{
    .number = "58.3",
},



{
    .number = "59",
},



{
    .number = "60",
},



{
    .number = "61.1",
},

{
    .number = "61.2",
},

{
    .number = "61.3",
},



{
    .number = "62",
},



{
    .number = "63.1",
},

{
    .number = "63.2",
},



{
    .number = "64.1",
},

{
    .number = "64.2",
},



{
    .number = "65.1",
},

{
    .number = "65.2",
},



{
    .number = "66.1",
},

{
    .number = "66.2",
},



{
    .number = "67.1",
},

{
    .number = "67.2",
},



{
    .number = "68",
},



{
    .number = "69",
},



{
    .number = "70",
},



{
    .number = "71.1",
},

{
    .number = "71.2",
},



{
    .number = "72.1",
},

{
    .number = "72.2",
},



{
    .number = "73.1",
},

{
    .number = "73.2",
},



{
    .number = "74.1",
},

{
    .number = "74.2",
},

{
    .number = "74.3",
},



{
    .number = "75.1",
},

{
    .number = "75.2",
},



{
    .number = "76",
},



{
    .number = "77.1",
},

{
    .number = "77.2",
},



{
    .number = "78",
},



{
    .number = "79",
},



{
    .number = "80.1",
},

{
    .number = "80.2",
},

{
    .number = "80.3",
},



{
    .number = "81",
},



{
    .number = "82",
},



{
    .number = "83",
},



{
    .number = "84",
},



{
    .number = "85.1",
},

{
    .number = "85.2",
},

{
    .number = "85.3",
},



{
    .number = "86.1",
},

{
    .number = "86.2",
},

{
    .number = "86.3",
},



{
    .number = "87.1",
},

{
    .number = "87.2",
},

{
    .number = "87.3",
},



{
    .number = "88.1",
},



{
    .number = "89.1",
},

{
    .number = "89.2",
},



{
    .number = "90",
},



{
    .number = "91",
},



{
    .number = "92",
},



{
    .number = "93.1",
},

{
    .number = "93.2",
},



{
    .number = "94.1",
},

{
    .number = "94.2",
},



{
    .number = "95",
},



{
    .number = "96.1",
},

{
    .number = "96.2",
},



{
    .number = "97",
},



{
    .number = "98",
},



{
    .number = "99",
},



{
    .number = "100",
},



{
    .number = "101",
},



{
    .number = "102",
},



{
    .number = "103.1",
},

{
    .number = "103.2",
},



{
    .number = "104",
},



{
    .number = "105",
},



{
    .number = "106",
},



{
    .number = "107",
},



{
    .number = "108",
},



{
    .number = "109",
},



{
    .number = "110",
},



{
    .number = "111.1",
},

{
    .number = "111.2",
},



{
    .number = "112.1",
},

{
    .number = "112.2",
},



{
    .number = "113.1",
},

{
    .number = "113.2",
},

{
    .number = "113.3",
},



{
    .number = "114",
},



{
    .number = "115.1",
},

{
    .number = "115.2",
},



{
    .number = "116.1",
},

{
    .number = "116.2",
},



{
    .number = "117.1",
},

{
    .number = "117.2",
},



{
    .number = "118.1",
},

{
    .number = "118.2",
},



{
    .number = "119.1",
},

{
    .number = "119.2",
},



{
    .number = "120.1",
},

{
    .number = "120.2",
},



{
    .number = "121",
},



{
    .number = "122.1",
},

{
    .number = "122.2",
},

{
    .number = "122.3",
},



{
    .number = "123.1",
},

{
    .number = "123.2",
},

{
    .number = "123.3",
},



{
    .number = "124.1",
},

{
    .number = "124.2",
},



{
    .number = "125.1",
},

{
    .number = "125.2",
},



{
    .number = "126",
},



{
    .number = "127.1",
},

{
    .number = "127.2",
},

{
    .number = "127.3",
},



{
    .number = "128.1",
},

{
    .number = "128.2",
},



{
    .number = "129",
},



{
    .number = "130.1",
},

{
    .number = "130.2",
},

{
    .number = "130.3",
},



{
    .number = "131",
},



{
    .number = "132",
},



{
    .number = "133",
},



{
    .number = "134",
},



{
    .number = "135.1",
},

{
    .number = "135.2",
},



{
    .number = "136.1",
},

{
    .number = "136.2",
},



{
    .number = "137.1",
},

{
    .number = "137.2",
},



{
    .number = "138",
},



{
    .number = "139.1",
},

{
    .number = "139.2",
},



{
    .number = "140.1",
},

{
    .number = "140.2",
},

{
    .number = "140.3",
},



{
    .number = "141",
},



{
    .number = "142.1",
},

{
    .number = "142.2",
},



{
    .number = "143.1",
},

{
    .number = "143.2",
},



{
    .number = "144.1",
},

{
    .number = "144.2",
},



{
    .number = "145.1",
},

{
    .number = "145.2",
},

{
    .number = "145.3",
},



{
    .number = "146.1",
},

{
    .number = "146.2",
},

{
    .number = "146.3",
},



{
    .number = "147",
},



{
    .number = "148.1",
},

{
    .number = "148.2",
},



{
    .number = "149.1",
},

{
    .number = "149.2",
},



{
    .number = "150.1",
},

{
    .number = "150.2",
},



{
    .number = "151.1",
},

{
    .number = "151.2",
},



{
    .number = "152.1",
},

{
    .number = "152.2",
},



{
    .number = "153",
},



{
    .number = "154.1",
},

{
    .number = "154.2",
},



{
    .number = "155.1",
},

{
    .number = "155.2",
},



{
    .number = "156.1",
},

{
    .number = "156.2",
},



{
    .number = "157.1",
},

{
    .number = "157.2",
},



{
    .number = "158.1",
},

{
    .number = "158.2",
},



{
    .number = "159.1",
},

{
    .number = "159.2",
},



{
    .number = "160.1",
},

{
    .number = "160.2",
},



{
    .number = "161",
},



{
    .number = "162.1",
},

{
    .number = "162.2",
},

{
    .number = "162.3",
},



{
    .number = "163.1",
},

{
    .number = "163.2",
},



{
    .number = "164.1",
},

{
    .number = "164.2",
},



{
    .number = "165.1",
},

{
    .number = "165.2",
},



{
    .number = "166.1",
},

{
    .number = "166.2",
},



{
    .number = "167.1",
},

{
    .number = "167.2",
},



{
    .number = "168.1",
},

{
    .number = "168.2",
},



{
    .number = "169",
},



{
    .number = "170.1",
},

{
    .number = "170.2",
},



{
    .number = "171",
},



{
    .number = "172",
},



{
    .number = "173.1",
},

{
    .number = "173.2",
},



{
    .number = "174.1",
},

{
    .number = "174.2",
},



{
    .number = "175",
},



{
    .number = "176",
},



{
    .number = "177.1",
},

{
    .number = "177.2",
},



{
    .number = "178",
},



{
    .number = "179",
},



{
    .number = "180.1",
},

{
    .number = "180.2",
},



{
    .number = "181",
},



{
    .number = "182.1",
},

{
    .number = "182.2",
},



{
    .number = "183",
},



{
    .number = "184.1",
},

{
    .number = "184.2",
},

{
    .number = "184.3",
},



{
    .number = "185",
},



{
    .number = "186",
},



{
    .number = "187.1",
},

{
    .number = "187.2",
},



{
    .number = "188.1",
},

{
    .number = "188.2",
},



{
    .number = "189",
},



{
    .number = "190",
},



{
    .number = "191",
},



{
    .number = "192",
},



{
    .number = "193.1",
},

{
    .number = "193.2",
},



{
    .number = "194.1",
},

{
    .number = "194.2",
},



{
    .number = "195.1",
},

{
    .number = "195.2",
},



{
    .number = "196.1",
},

{
    .number = "196.2",
},



{
    .number = "197",
},



{
    .number = "198",
},



{
    .number = "199.1",
},

{
    .number = "199.2",
},

{
    .number = "199.3",
},

{
    .number = "199.4",
},



{
    .number = "200.1",
},

{
    .number = "200.2",
},

{
    .number = "200.3",
},



{
    .number = "201.1",
},

{
    .number = "201.2",
},



{
    .number = "202",
},



{
    .number = "203.1",
},

{
    .number = "203.2",
},



{
    .number = "204",
},



{
    .number = "205",
},



{
    .number = "206",
},



{
    .number = "207",
},



{
    .number = "208",
},



{
    .number = "209.1",
},

{
    .number = "209.2",
},

{
    .number = "209.3",
},



{
    .number = "210.1",
},

{
    .number = "210.2",
},



{
    .number = "211.1",
},

{
    .number = "211.2",
},

{
    .number = "211.4",
},



{
    .number = "212.1",
},

{
    .number = "212.2",
},



{
    .number = "213.1",
},

{
    .number = "213.2",
},



{
    .number = "214",
},



