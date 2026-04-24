<!-- Login Page -->
<!DOCTYPE html>

<html dir="rtl" lang="ar"><head>
<meta charset="utf-8"/>
<meta content="width=device-width, initial-scale=1.0" name="viewport"/>
<title>تسجيل الدخول - نديم</title>
<script src="https://cdn.tailwindcss.com?plugins=forms,container-queries"></script>
<link href="https://fonts.googleapis.com/css2?family=Baloo+Bhaijaan+2:wght@400;700;800&amp;family=Cairo:wght@400;600;700&amp;display=swap" rel="stylesheet"/>
<link href="https://fonts.googleapis.com/css2?family=Material+Symbols+Outlined:wght,FILL@100..700,0..1&amp;display=swap" rel="stylesheet"/>
<link href="https://fonts.googleapis.com/css2?family=Material+Symbols+Outlined:wght,FILL@100..700,0..1&amp;display=swap" rel="stylesheet"/>
<script id="tailwind-config">
        tailwind.config = {
            darkMode: "class",
            theme: {
                extend: {
                    colors: {
                        "tertiary-fixed-dim": "#d6c953",
                        "inverse-primary": "#7cd0ff",
                        "on-primary-container": "#00455f",
                        "tertiary-container": "#baae3b",
                        "on-background": "#1a1c19",
                        "on-surface-variant": "#3e484f",
                        "secondary-container": "#fdaa3f",
                        "surface-dim": "#dadad5",
                        "on-primary-fixed": "#001e2c",
                        "secondary-fixed": "#ffddba",
                        "surface": "#fafaf5",
                        "on-primary": "#ffffff",
                        "surface-container-high": "#e8e8e3",
                        "surface-bright": "#fafaf5",
                        "inverse-surface": "#2f312e",
                        "on-tertiary-fixed-variant": "#4e4800",
                        "primary-fixed-dim": "#7cd0ff",
                        "on-primary-fixed-variant": "#004c69",
                        "error-container": "#ffdad6",
                        "outline-variant": "#bec8d0",
                        "secondary": "#875200",
                        "primary": "#00668a",
                        "on-secondary": "#ffffff",
                        "tertiary": "#686000",
                        "on-secondary-fixed-variant": "#673d00",
                        "on-error-container": "#93000a",
                        "secondary-fixed-dim": "#ffb866",
                        "surface-container-low": "#f4f4ef",
                        "surface-container-highest": "#e3e3de",
                        "surface-container": "#eeeee9",
                        "tertiary-fixed": "#f3e56c",
                        "on-secondary-container": "#6d4100",
                        "outline": "#6f7880",
                        "on-tertiary-fixed": "#1f1c00",
                        "on-secondary-fixed": "#2b1700",
                        "on-tertiary-container": "#474100",
                        "primary-container": "#4fb6e9",
                        "primary-fixed": "#c4e7ff",
                        "on-error": "#ffffff",
                        "on-surface": "#1a1c19",
                        "surface-variant": "#e3e3de",
                        "on-tertiary": "#ffffff",
                        "surface-tint": "#00668a",
                        "background": "#fafaf5",
                        "surface-container-lowest": "#ffffff",
                        "inverse-on-surface": "#f1f1ec",
                        "error": "#ba1a1a"
                    },
                    borderRadius: {
                        "DEFAULT": "1rem",
                        "lg": "2rem",
                        "xl": "3rem",
                        "full": "9999px"
                    },
                    fontFamily: {
                        "headline": ["'Baloo Bhaijaan 2'", "sans-serif"],
                        "body": ["'Cairo'", "sans-serif"],
                        "label": ["'Cairo'", "sans-serif"]
                    }
                }
            }
        }
    </script>
<style>
        .texture-overlay {
            background-image: url("data:image/svg+xml,%3Csvg viewBox='0 0 200 200' xmlns='http://www.w3.org/2000/svg'%3E%3Cfilter id='noiseFilter'%3E%3CfeTurbulence type='fractalNoise' baseFrequency='0.65' numOctaves='3' stitchTiles='stitch'/%3E%3C/filter%3E%3Crect width='100%25' height='100%25' filter='url(%23noiseFilter)' opacity='0.02'/%3E%3C/svg%3E");
            pointer-events: none;
        }
    </style>
</head>
<body class="bg-background text-on-background font-body min-h-screen flex items-center justify-center relative overflow-hidden">
<div class="absolute inset-0 texture-overlay z-0"></div>
<!-- Decorative background elements -->
<div class="absolute top-[-10%] right-[-5%] w-96 h-96 bg-primary-container rounded-full mix-blend-multiply filter blur-3xl opacity-30 z-0"></div>
<div class="absolute bottom-[-10%] left-[-5%] w-80 h-80 bg-secondary-container rounded-full mix-blend-multiply filter blur-3xl opacity-30 z-0"></div>
<div class="relative z-10 w-full max-w-md px-6 py-12">
<div class="bg-surface-container-highest rounded-xl p-10 shadow-[0_8px_30px_rgb(0,0,0,0.04)] relative overflow-hidden">
<!-- Header / Logo -->
<div class="text-center mb-10">
<div class="flex justify-center mb-2"><img alt="نديم" class="h-40 w-auto object-contain" src="https://lh3.googleusercontent.com/aida/ADBb0uhb3K05xBavCVb8YCsNHNxC0dDQA0iQEA1KfFM9iICU-JcTNcetpLmTTfmqcDUgR8rweEGZL_FhQHSM5syLac5m6Ur3xoSv8bP8jNyNT5PG4r0SHMsHsHp8fh68Nu-ELeKvlmkBkY7adaDzAwYGW4VhS2ExRn38Brsa2cyqIVEo7Yt6ri4Hczz1BjVyJDShwPyhot3-F3RQgkecwwh9sflPZgczN1ZN4ASR6uCQWS4RUe4my1mkf3uxlZ-wNyheZLhPixhadMOUf4c"/></div>
<p class="font-body text-on-surface-variant mt-2 text-lg">مرحباً بك في عالم الخيال</p>
</div>
<!-- Login Form -->
<form class="space-y-6">
<!-- Email Field -->
<div>
<label class="block font-label text-sm font-semibold text-on-surface mb-2" for="email">البريد الإلكتروني</label>
<div class="relative">
<div class="absolute inset-y-0 right-0 pr-4 flex items-center pointer-events-none">
<span class="material-symbols-outlined text-outline" data-icon="mail">mail</span>
</div>
<input class="block w-full rounded-md border-0 py-3 pr-12 pl-4 text-on-surface shadow-sm ring-1 ring-inset ring-outline-variant placeholder:text-outline focus:ring-2 focus:ring-inset focus:ring-primary focus:bg-surface-bright transition-all bg-surface-container-lowest font-body" id="email" placeholder="name@example.com" type="email"/>
</div>
</div>
<!-- Password Field -->
<div>
<div class="flex justify-between items-center mb-2">
<label class="block font-label text-sm font-semibold text-on-surface" for="password">كلمة المرور</label>
<a class="font-label text-sm text-primary hover:text-primary-container transition-colors" href="#">نسيت كلمة المرور؟</a>
</div>
<div class="relative">
<div class="absolute inset-y-0 right-0 pr-4 flex items-center pointer-events-none">
<span class="material-symbols-outlined text-outline" data-icon="lock">lock</span>
</div>
<input class="block w-full rounded-md border-0 py-3 pr-12 pl-4 text-on-surface shadow-sm ring-1 ring-inset ring-outline-variant placeholder:text-outline focus:ring-2 focus:ring-inset focus:ring-primary focus:bg-surface-bright transition-all bg-surface-container-lowest font-body" id="password" placeholder="••••••••" type="password"/>
</div>
</div>
<!-- Action Button -->
<button class="w-full flex justify-center py-4 px-4 border border-transparent rounded-full shadow-sm text-lg font-headline font-bold text-on-primary bg-gradient-to-br from-primary to-primary-container hover:scale-[1.02] transition-transform focus:outline-none focus:ring-2 focus:ring-offset-2 focus:ring-primary mt-8" type="submit">
                    تسجيل الدخول
                </button>
</form>
<div class="mt-8 text-center">
<p class="font-body text-sm text-on-surface-variant">
                    ليس لديك حساب؟ <a class="font-bold text-primary hover:text-primary-container transition-colors" href="#">إنشاء حساب جديد</a>
</p>
</div>
</div>
</div>
</body></html>

<!-- Dashboard Home -->
<!DOCTYPE html>

<html class="light" dir="rtl" lang="ar"><head>
<meta charset="utf-8"/>
<meta content="width=device-width, initial-scale=1.0" name="viewport"/>
<title>نديم - الرئيسية</title>
<script src="https://cdn.tailwindcss.com?plugins=forms,container-queries"></script>
<link href="https://fonts.googleapis.com/css2?family=Baloo+Bhaijaan+2:wght@400..800&amp;family=Cairo:wght@400..800&amp;family=Material+Symbols+Outlined:wght,FILL@100..700,0..1&amp;display=swap" rel="stylesheet"/>
<link href="https://fonts.googleapis.com/css2?family=Material+Symbols+Outlined:wght,FILL@100..700,0..1&amp;display=swap" rel="stylesheet"/>
<style>
        .font-headline { font-family: 'Baloo Bhaijaan 2', cursive; }
        .font-body { font-family: 'Cairo', sans-serif; }
    </style>
<script id="tailwind-config">
        tailwind.config = {
          darkMode: "class",
          theme: {
            extend: {
              "colors": {
                      "tertiary-fixed-dim": "#d6c953",
                      "inverse-primary": "#7cd0ff",
                      "on-primary-container": "#00455f",
                      "tertiary-container": "#baae3b",
                      "on-background": "#1a1c19",
                      "on-surface-variant": "#3e484f",
                      "secondary-container": "#fdaa3f",
                      "surface-dim": "#dadad5",
                      "on-primary-fixed": "#001e2c",
                      "secondary-fixed": "#ffddba",
                      "surface": "#fafaf5",
                      "on-primary": "#ffffff",
                      "surface-container-high": "#e8e8e3",
                      "surface-bright": "#fafaf5",
                      "inverse-surface": "#2f312e",
                      "on-tertiary-fixed-variant": "#4e4800",
                      "primary-fixed-dim": "#7cd0ff",
                      "on-primary-fixed-variant": "#004c69",
                      "error-container": "#ffdad6",
                      "outline-variant": "#bec8d0",
                      "secondary": "#875200",
                      "primary": "#00668a",
                      "on-secondary": "#ffffff",
                      "tertiary": "#686000",
                      "on-secondary-fixed-variant": "#673d00",
                      "on-error-container": "#93000a",
                      "secondary-fixed-dim": "#ffb866",
                      "surface-container-low": "#f4f4ef",
                      "surface-container-highest": "#e3e3de",
                      "surface-container": "#eeeee9",
                      "tertiary-fixed": "#f3e56c",
                      "on-secondary-container": "#6d4100",
                      "outline": "#6f7880",
                      "on-tertiary-fixed": "#1f1c00",
                      "on-secondary-fixed": "#2b1700",
                      "on-tertiary-container": "#474100",
                      "primary-container": "#4fb6e9",
                      "primary-fixed": "#c4e7ff",
                      "on-error": "#ffffff",
                      "on-surface": "#1a1c19",
                      "surface-variant": "#e3e3de",
                      "on-tertiary": "#ffffff",
                      "surface-tint": "#00668a",
                      "background": "#fafaf5",
                      "surface-container-lowest": "#ffffff",
                      "inverse-on-surface": "#f1f1ec",
                      "error": "#ba1a1a"
              },
              "borderRadius": {
                      "DEFAULT": "1rem",
                      "lg": "2rem",
                      "xl": "3rem",
                      "full": "9999px"
              },
              "fontFamily": {
                      "headline": ["'Baloo Bhaijaan 2'", "cursive"],
                      "body": ["'Cairo'", "sans-serif"],
                      "label": ["'Cairo'", "sans-serif"]
              }
            },
          },
        }
    </script>
</head>
<body class="bg-background text-on-background font-body antialiased min-h-screen relative rtl">
<div class="bg-[#fafaf5]/80 dark:bg-slate-950/80 backdrop-blur-xl docked w-full top-0 h-20 fixed top-0 left-0 right-0 z-30 flex justify-between items-center px-12 rtl border-b-0 shadow-sm shadow-surface-variant/50">
<div class="font-['Baloo_Bhaijaan_2'] text-2xl font-black text-[#00668a]">
            نديم
        </div>
<div class="flex items-center space-x-6 space-x-reverse">
<button class="text-primary hover:opacity-80 transition-opacity">
<span class="material-symbols-outlined" data-icon="notifications">notifications</span>
</button>
<button class="text-primary hover:opacity-80 transition-opacity">
<span class="material-symbols-outlined" data-icon="child_care">child_care</span>
</button>
<div class="h-10 w-10 rounded-full bg-surface-container-highest overflow-hidden border-2 border-transparent outline outline-2 outline-offset-2 outline-primary/20">
<img alt="صورة المستخدم" class="w-full h-full object-cover" data-alt="close up portrait of a young arab father with a gentle smile in soft warm indoor lighting" src="https://lh3.googleusercontent.com/aida-public/AB6AXuAxBfurT6xETjeg8HNPwHjMgt9NlB2Ug6gpwsO_f1kX3Del_ZYbm-aYPXJtNaH5RDoGsjtOvKlOqSLOgoFFWv9L_2I9h1AbuVDitwwDJUeKliPeOBWvs8GoAu4d1ry4VfiG2Hw3Lk61w8Kk_p-sJPon9ztCTA2IG2pQtZSsaYOigYU3D6edWVzRd5_jrfdvOa12SUwQF-MshIdkaAzpcGMcySM7GVOqwBQfZFA5uC4GM9Z-ohOWIsj3vpjQwMufhUBAkIsgVnvb2NiY"/>
</div>
</div>
</div>
<div class="fixed right-0 top-0 h-full z-40 flex flex-col p-6 space-y-8 rtl bg-slate-50 dark:bg-slate-900 docked w-72 border-l-0 rounded-l-[3rem] shadow-[-4px_0_24px_rgba(0,0,0,0.02)] pt-28">
<div class="flex flex-col items-center mb-8">
<div class="h-20 w-20 rounded-full bg-surface-container-highest overflow-hidden mb-3 bg-white shadow-sm">
<img alt="نديم شعار" class="w-full h-full object-contain p-1" src="https://lh3.googleusercontent.com/aida/ADBb0uhb3K05xBavCVb8YCsNHNxC0dDQA0iQEA1KfFM9iICU-JcTNcetpLmTTfmqcDUgR8rweEGZL_FhQHSM5syLac5m6Ur3xoSv8bP8jNyNT5PG4r0SHMsHsHp8fh68Nu-ELeKvlmkBkY7adaDzAwYGW4VhS2ExRn38Brsa2cyqIVEo7Yt6ri4Hczz1BjVyJDShwPyhot3-F3RQgkecwwh9sflPZgczN1ZN4ASR6uCQWS4RUe4my1mkf3uxlZ-wNyheZLhPixhadMOUf4c"/>
</div>
<h2 class="font-['Baloo_Bhaijaan_2'] text-3xl font-bold text-[#00668a] dark:text-[#4fb6e9]">نديم</h2>
<p class="font-['Cairo'] text-sm font-medium tracking-wide text-on-surface-variant">عالم من الخيال</p>
</div>
<nav class="flex-1 flex flex-col space-y-2">
<a class="flex items-center space-x-4 space-x-reverse px-6 py-4 bg-gradient-to-br from-[#00668a] to-[#4fb6e9] text-white rounded-full shadow-[0_8px_30px_rgb(0,102,138,0.2)] scale-105 font-['Cairo'] text-sm font-medium tracking-wide transition-transform ease-out" href="#">
<span class="material-symbols-outlined" data-weight="fill" style="font-variation-settings: 'FILL' 1;">home</span>
<span>الرئيسية</span>
</a>
<a class="flex items-center space-x-4 space-x-reverse px-6 py-4 text-slate-600 dark:text-slate-400 hover:text-[#00668a] hover:bg-slate-100 dark:hover:bg-slate-800 transition-all duration-300 rounded-full font-['Cairo'] text-sm font-medium tracking-wide" href="#">
<span class="material-symbols-outlined">devices</span>
<span>أجهزتي</span>
</a>
<a class="flex items-center space-x-4 space-x-reverse px-6 py-4 text-slate-600 dark:text-slate-400 hover:text-[#00668a] hover:bg-slate-100 dark:hover:bg-slate-800 transition-all duration-300 rounded-full font-['Cairo'] text-sm font-medium tracking-wide" href="#">
<span class="material-symbols-outlined">library_music</span>
<span>مكتبة السماسم</span>
</a>
<a class="flex items-center space-x-4 space-x-reverse px-6 py-4 text-slate-600 dark:text-slate-400 hover:text-[#00668a] hover:bg-slate-100 dark:hover:bg-slate-800 transition-all duration-300 rounded-full font-['Cairo'] text-sm font-medium tracking-wide" href="#">
<span class="material-symbols-outlined">forum</span>
<span>سجل المحادثات الذكية</span>
</a>
<a class="flex items-center space-x-4 space-x-reverse px-6 py-4 text-slate-600 dark:text-slate-400 hover:text-[#00668a] hover:bg-slate-100 dark:hover:bg-slate-800 transition-all duration-300 rounded-full font-['Cairo'] text-sm font-medium tracking-wide mt-auto" href="#">
<span class="material-symbols-outlined">settings</span>
<span>الإعدادات</span>
</a>
</nav>
</div>
<main class="mr-72 pt-28 px-8 pb-12">
<header class="mb-10">
<h1 class="font-headline text-[2.5rem] font-bold text-primary mb-2">مرحباً، أهالي نديم!</h1>
<p class="text-on-surface-variant text-lg">إليكم نظرة سريعة على نشاط طفلكم اليوم.</p>
</header>
<div class="grid grid-cols-1 md:grid-cols-3 gap-6 mb-12">
<div class="bg-surface-container-highest rounded-xl p-8 relative overflow-hidden group hover:scale-[1.02] hover:bg-surface-bright transition-all duration-300">
<div class="absolute -right-6 -top-6 w-32 h-32 bg-primary/5 rounded-full blur-2xl"></div>
<div class="flex items-start justify-between mb-6 relative z-10">
<div class="h-14 w-14 rounded-full bg-primary-container/20 flex items-center justify-center text-primary">
<span class="material-symbols-outlined text-3xl">headphones</span>
</div>
</div>
<div class="relative z-10">
<p class="text-on-surface-variant text-sm font-medium mb-1">إجمالي ساعات الاستماع</p>
<h3 class="font-headline text-4xl font-bold text-primary">14 <span class="text-xl text-on-surface-variant font-body font-normal">ساعة</span></h3>
</div>
</div>
<div class="bg-surface-container-highest rounded-xl p-8 relative overflow-hidden group hover:scale-[1.02] hover:bg-surface-bright transition-all duration-300">
<div class="absolute -right-6 -top-6 w-32 h-32 bg-secondary-container/10 rounded-full blur-2xl"></div>
<div class="flex items-start justify-between mb-6 relative z-10">
<div class="h-14 w-14 rounded-full bg-secondary-container/20 flex items-center justify-center text-secondary">
<span class="material-symbols-outlined text-3xl">smart_toy</span>
</div>
</div>
<div class="relative z-10">
<p class="text-on-surface-variant text-sm font-medium mb-1">السماسم المملوكة</p>
<h3 class="font-headline text-4xl font-bold text-primary">8 <span class="text-xl text-on-surface-variant font-body font-normal">سماسم</span></h3>
</div>
</div>
<div class="bg-surface-container-highest rounded-xl p-8 relative overflow-hidden group hover:scale-[1.02] hover:bg-surface-bright transition-all duration-300">
<div class="absolute -right-6 -top-6 w-32 h-32 bg-tertiary-container/10 rounded-full blur-2xl"></div>
<div class="flex items-start justify-between mb-6 relative z-10">
<div class="h-14 w-14 rounded-full bg-surface-container flex items-center justify-center text-on-surface-variant">
<span class="material-symbols-outlined text-3xl">wifi</span>
</div>
<div class="flex items-center space-x-2 space-x-reverse bg-surface px-3 py-1.5 rounded-full shadow-sm">
<div class="w-2.5 h-2.5 rounded-full bg-green-500 animate-pulse"></div>
<span class="text-xs font-bold text-green-700">متصل</span>
</div>
</div>
<div class="relative z-10">
<p class="text-on-surface-variant text-sm font-medium mb-1">حالة الجهاز</p>
<h3 class="font-headline text-2xl font-bold text-primary mt-2">جهاز نديم الأساسي</h3>
</div>
</div>
</div>
</main>
</body></html>

<!-- My Devices (أجهزتي) -->
<!DOCTYPE html>

<html dir="rtl" lang="ar"><head>
<meta charset="utf-8"/>
<meta content="width=device-width, initial-scale=1.0" name="viewport"/>
<title>أجهزتي - لوحة تحكم نديم</title>
<script src="https://cdn.tailwindcss.com?plugins=forms,container-queries"></script>
<link href="https://fonts.googleapis.com/css2?family=Material+Symbols+Outlined:wght,FILL@100..700,0..1&amp;display=swap" rel="stylesheet"/>
<link href="https://fonts.googleapis.com/css2?family=Baloo+Bhaijaan+2:wght@400;500;600;700;800&amp;family=Cairo:wght@300;400;500;600;700;800&amp;display=swap" rel="stylesheet"/>
<link href="https://fonts.googleapis.com/css2?family=Material+Symbols+Outlined:wght,FILL@100..700,0..1&amp;display=swap" rel="stylesheet"/>
<script id="tailwind-config">
  tailwind.config = {
    darkMode: "class",
    theme: {
      extend: {
        "colors": {
                "tertiary-fixed": "#f3e56c",
                "on-primary-container": "#00455f",
                "primary-fixed": "#c4e7ff",
                "surface-container-highest": "#e3e3de",
                "on-background": "#1a1c19",
                "outline-variant": "#bec8d0",
                "on-surface-variant": "#3e484f",
                "error": "#ba1a1a",
                "secondary-fixed": "#ffddba",
                "on-secondary": "#ffffff",
                "on-error-container": "#93000a",
                "background": "#fafaf5",
                "inverse-on-surface": "#f1f1ec",
                "on-secondary-fixed": "#2b1700",
                "primary": "#00668a",
                "on-tertiary-fixed": "#1f1c00",
                "tertiary-fixed-dim": "#d6c953",
                "surface-container-low": "#f4f4ef",
                "inverse-primary": "#7cd0ff",
                "surface-container-lowest": "#ffffff",
                "outline": "#6f7880",
                "on-tertiary-container": "#474100",
                "primary-container": "#4fb6e9",
                "on-tertiary": "#ffffff",
                "tertiary-container": "#baae3b",
                "surface-bright": "#fafaf5",
                "surface-variant": "#e3e3de",
                "on-tertiary-fixed-variant": "#4e4800",
                "secondary": "#875200",
                "on-error": "#ffffff",
                "on-secondary-container": "#6d4100",
                "primary-fixed-dim": "#7cd0ff",
                "surface-dim": "#dadad5",
                "surface": "#fafaf5",
                "secondary-fixed-dim": "#ffb866",
                "inverse-surface": "#2f312e",
                "surface-container-high": "#e8e8e3",
                "on-primary-fixed-variant": "#004c69",
                "tertiary": "#686000",
                "surface-container": "#eeeee9",
                "on-primary": "#ffffff",
                "on-primary-fixed": "#001e2c",
                "secondary-container": "#fdaa3f",
                "surface-tint": "#00668a",
                "error-container": "#ffdad6",
                "on-secondary-fixed-variant": "#673d00",
                "on-surface": "#1a1c19"
        },
        "borderRadius": {
                "DEFAULT": "1rem",
                "lg": "2rem",
                "xl": "3rem",
                "full": "9999px"
        },
        "spacing": {},
        "fontFamily": {
                "headline": [
                        "Baloo Bhaijaan 2"
                ],
                "body": [
                        "Cairo"
                ],
                "label": [
                        "Cairo"
                ]
        }
},
    },
  }
</script>
<style>
  body {
    font-family: 'Cairo', sans-serif;
  }
  h1, h2, h3, h4, h5, h6, .font-headline {
    font-family: 'Baloo Bhaijaan 2', sans-serif;
  }
  .texture-bg {
    position: relative;
  }
  .texture-bg::before {
    content: "";
    position: absolute;
    inset: 0;
    background-image: url('data:image/svg+xml;utf8,%3Csvg viewBox="0 0 200 200" xmlns="http://www.w3.org/2000/svg"%3E%3Cfilter id="noiseFilter"%3E%3CfeTurbulence type="fractalNoise" baseFrequency="0.65" numOctaves="3" stitchTiles="stitch"/%3E%3C/filter%3E%3Crect width="100%25" height="100%25" filter="url(%23noiseFilter)" opacity="0.02"/%3E%3C/svg%3E');
    pointer-events: none;
  }
</style>
</head>
<body class="bg-background text-on-surface min-h-screen flex selection:bg-primary-container selection:text-on-primary-container">
<!-- SideNavBar Web -->
<aside class="hidden md:flex flex-col bg-[#fafaf5] dark:bg-slate-900 border-l border-slate-100 dark:border-slate-800 shadow-xl shadow-[#00668a]/5 h-full w-72 fixed right-0 top-0 pt-20 pb-8 z-30 transition-transform translate-x-0">
<div class="px-6 mb-10 text-center"><img alt="نديم" class="h-32 mx-auto object-contain drop-shadow-sm mb-4" src="https://lh3.googleusercontent.com/aida/ADBb0uhb3K05xBavCVb8YCsNHNxC0dDQA0iQEA1KfFM9iICU-JcTNcetpLmTTfmqcDUgR8rweEGZL_FhQHSM5syLac5m6Ur3xoSv8bP8jNyNT5PG4r0SHMsHsHp8fh68Nu-ELeKvlmkBkY7adaDzAwYGW4VhS2ExRn38Brsa2cyqIVEo7Yt6ri4Hczz1BjVyJDShwPyhot3-F3RQgkecwwh9sflPZgczN1ZN4ASR6uCQWS4RUe4my1mkf3uxlZ-wNyheZLhPixhadMOUf4c"/></div>
<nav class="flex-1 space-y-2">
<a class="text-slate-600 dark:text-slate-400 mx-4 py-3 px-6 flex items-center gap-3 font-['Cairo'] font-medium hover:bg-[#4fb6e9]/10 rounded-full transition-all" href="#">
<span class="material-symbols-outlined" style="font-variation-settings: 'FILL' 0;">home</span>
        الرئيسية
      </a>
<a class="bg-gradient-to-r from-[#00668a] to-[#4fb6e9] text-white rounded-full mx-4 py-3 px-6 flex items-center gap-3 font-['Cairo'] font-medium shadow-md shadow-primary/20 hover:scale-105 transition-transform" href="#">
<span class="material-symbols-outlined" style="font-variation-settings: 'FILL' 1;">devices</span>
        أجهزتي
      </a>
<a class="text-slate-600 dark:text-slate-400 mx-4 py-3 px-6 flex items-center gap-3 font-['Cairo'] font-medium hover:bg-[#4fb6e9]/10 rounded-full transition-all" href="#">
<span class="material-symbols-outlined" style="font-variation-settings: 'FILL' 0;">library_music</span>
        المكتبة
      </a>
<a class="text-slate-600 dark:text-slate-400 mx-4 py-3 px-6 flex items-center gap-3 font-['Cairo'] font-medium hover:bg-[#4fb6e9]/10 rounded-full transition-all" href="#">
<span class="material-symbols-outlined" style="font-variation-settings: 'FILL' 0;">settings</span>
        الإعدادات
      </a>
</nav>
<div class="px-6 mt-auto">
<button class="w-full bg-secondary-container text-on-secondary-container rounded-full py-4 font-['Cairo'] font-bold hover:scale-105 transition-transform flex items-center justify-center gap-2 shadow-sm">
<span class="material-symbols-outlined">add</span>
        إضافة جهاز جديد
      </button>
</div>
</aside>
<!-- Main Content Area -->
<main class="flex-1 md:mr-72 min-h-screen flex flex-col w-full pb-24 md:pb-0">
<!-- TopNavBar Mobile & Web -->
<header class="bg-[#fafaf5] dark:bg-slate-900 bg-surface-container-low flex flex-row-reverse justify-between items-center w-full px-6 py-4 docked full-width top-0 z-40 md:sticky sticky">
<div class="flex items-center gap-4 text-[#00668a] dark:text-[#4fb6e9]">
<button class="hover:bg-slate-100 dark:hover:bg-slate-800 transition-colors p-2 rounded-full md:hidden">
<span class="material-symbols-outlined">menu</span>
</button>
<img alt="نديم" class="h-12 w-auto object-contain" src="https://lh3.googleusercontent.com/aida/ADBb0uhb3K05xBavCVb8YCsNHNxC0dDQA0iQEA1KfFM9iICU-JcTNcetpLmTTfmqcDUgR8rweEGZL_FhQHSM5syLac5m6Ur3xoSv8bP8jNyNT5PG4r0SHMsHsHp8fh68Nu-ELeKvlmkBkY7adaDzAwYGW4VhS2ExRn38Brsa2cyqIVEo7Yt6ri4Hczz1BjVyJDShwPyhot3-F3RQgkecwwh9sflPZgczN1ZN4ASR6uCQWS4RUe4my1mkf3uxlZ-wNyheZLhPixhadMOUf4c"/>
</div>
<div class="flex items-center gap-2">
<button class="hover:bg-slate-100 dark:hover:bg-slate-800 transition-colors p-2 rounded-full text-slate-500 dark:text-slate-400">
<span class="material-symbols-outlined">notifications</span>
</button>
<button class="hover:bg-slate-100 dark:hover:bg-slate-800 transition-colors p-2 rounded-full text-slate-500 dark:text-slate-400">
<span class="material-symbols-outlined">settings</span>
</button>
<img alt="صورة الملف الشخصي" class="w-10 h-10 rounded-full object-cover border-2 border-surface-container-highest md:hidden ml-2" data-alt="close up of a smiling cartoon avatar face with colorful background, warm and friendly, high quality 3d render style" src="https://lh3.googleusercontent.com/aida-public/AB6AXuAbazt95-OcteGD-bvY_c85MaV65os945QC_fa3wLm9Dhd4Qh8eoUSIwFM2Y6qFw2W4VDmuZBsdiOjaYVCQ6004Au0HWy2IebCl-a-9tKpjYdbnCbwxWVU_rS1SqSxA8SI5Sbhfd78HI4irKHHF5ud3ZJJWM_L2kHJ2a0HOT9VVVQeChIdH-xa4vRsSh1sss-G4eNCLipxU4xdSkHiXoBHtVukwVltd9XbG2hX_srCE8vF_x01uV_BIAk8RoJuoPQhCoBHglKd-sloe"/>
</div>
</header>
<!-- Page Canvas -->
<div class="flex-1 p-6 md:p-10 lg:p-12 max-w-7xl mx-auto w-full">
<!-- Header -->
<div class="mb-12">
<h1 class="font-headline text-5xl text-primary font-bold mb-4 tracking-wide">أجهزتي</h1>
<p class="font-body text-lg text-on-surface-variant max-w-2xl">أدر أجهزة نديم الخاصة بك، تحقق من حالتها، واكتشف خيارات جديدة لتجربة استماع أكثر متعة لأطفالك.</p>
</div>
<!-- Main Devices Section (Bento Grid) -->
<div class="mb-16">
<h2 class="font-headline text-3xl text-primary-fixed-variant mb-6 flex items-center gap-3">
<span class="material-symbols-outlined" style="font-variation-settings: 'FILL' 1;">widgets</span>
          الأجهزة المفعلة
        </h2>
<div class="grid grid-cols-1 lg:grid-cols-12 gap-8">
<!-- Nadeem Smart Box - Hero Card -->
<div class="lg:col-span-8 bg-surface-container-highest rounded-xl p-8 relative overflow-hidden group hover:scale-[1.01] transition-transform duration-300">
<!-- Background Decorative -->
<div class="absolute -right-20 -top-20 w-64 h-64 bg-primary-container/20 rounded-full blur-3xl"></div>
<div class="absolute -left-10 -bottom-10 w-48 h-48 bg-secondary-container/20 rounded-full blur-2xl"></div>
<div class="flex flex-col md:flex-row gap-8 relative z-10">
<!-- Image Area -->
<div class="w-full md:w-2/5 aspect-square bg-surface rounded-lg flex items-center justify-center p-6 shadow-sm relative texture-bg overflow-hidden">
<img alt="صندوق نديم الذكي" class="w-full h-full object-contain drop-shadow-xl z-10" data-alt="minimalist smart speaker device in soft warm lighting, sleek rounded corners, matte finish, resting on a clean wooden table, light aesthetic" src="https://lh3.googleusercontent.com/aida-public/AB6AXuDrTkwuqJSxhnlu7FEaa72eABSubW_WyW6CmyhFUIm6MKJYQharoUGlfIr72fvzrdsZjdqgylHnvs2q-bxsPIW9NJfqiWHiYaoBo2rFo0zIJM2aLOWb5cnjq_mCp4qxbC7gFMrOK8b-DzgbNItwFm0RxJNTa4bezIemVlBpBmvVThumWMolKOIeBg1oSDKZFPMYDkOQyJNuvDPXDMFgdY-ZYjUSlf5EEUXCKnrMop2pNW4aNS4ZIZ7pghK3ksNL33mLX7HfNTxH54rL"/>
<!-- Status Glow -->
<div class="absolute bottom-6 w-20 h-2 bg-primary blur-xl opacity-40"></div>
</div>
<!-- Content Area -->
<div class="w-full md:w-3/5 flex flex-col justify-between">
<div>
<div class="flex justify-between items-start mb-2">
<h3 class="font-headline text-3xl text-on-background font-bold">صندوق نديم الذكي</h3>
<div class="bg-surface px-4 py-1.5 rounded-full flex items-center gap-2 shadow-sm text-sm font-medium text-emerald-600">
<span class="w-2.5 h-2.5 rounded-full bg-emerald-500 animate-pulse"></span>
                      متصل
                    </div>
</div>
<p class="font-body text-on-surface-variant mb-6">غرفة ألعاب يوسف</p>
<!-- Status Indicators -->
<div class="grid grid-cols-2 gap-4 mb-8">
<div class="bg-surface rounded-lg p-4 flex items-center gap-4">
<div class="w-10 h-10 rounded-full bg-primary/10 flex items-center justify-center text-primary">
<span class="material-symbols-outlined" style="font-variation-settings: 'FILL' 1;">battery_charging_80</span>
</div>
<div>
<p class="text-xs text-on-surface-variant font-medium">البطارية</p>
<p class="font-headline text-lg text-on-background font-bold">85%</p>
</div>
</div>
<div class="bg-surface rounded-lg p-4 flex items-center gap-4">
<div class="w-10 h-10 rounded-full bg-primary/10 flex items-center justify-center text-primary">
<span class="material-symbols-outlined" style="font-variation-settings: 'FILL' 1;">wifi</span>
</div>
<div>
<p class="text-xs text-on-surface-variant font-medium">إشارة الواي فاي</p>
<p class="font-headline text-lg text-on-background font-bold">ممتازة</p>
</div>
</div>
</div>
</div>
<!-- Actions -->
<div class="flex flex-wrap gap-4 mt-auto">
<button class="bg-gradient-to-r from-primary to-primary-container text-on-primary font-body font-bold py-3 px-6 rounded-full flex items-center gap-2 hover:scale-105 transition-transform shadow-md shadow-primary/20">
<span class="material-symbols-outlined" style="font-variation-settings: 'FILL' 1;">settings</span>
                    إعدادات الجهاز
                  </button>
<button class="bg-transparent text-primary hover:bg-surface-container-high font-body font-bold py-3 px-6 rounded-full flex items-center gap-2 transition-colors">
<span class="material-symbols-outlined">system_update</span>
                    تحديث النظام
                  </button>
</div>
</div>
</div>
</div>
<!-- Add Device Card (Glassmorphism inspired) -->
<div class="lg:col-span-4 bg-surface-container-lowest rounded-xl p-8 border border-outline-variant/15 flex flex-col items-center justify-center text-center relative overflow-hidden group hover:bg-surface transition-colors duration-300">
<div class="w-24 h-24 rounded-full bg-secondary-container/20 flex items-center justify-center mb-6 group-hover:scale-110 transition-transform duration-300">
<button class="w-16 h-16 rounded-full bg-secondary-container text-on-secondary-container flex items-center justify-center shadow-md shadow-secondary-container/30 hover:scale-105 transition-transform">
<span class="material-symbols-outlined text-4xl">add</span>
</button>
</div>
<h3 class="font-headline text-2xl text-on-background font-bold mb-2">إضافة جهاز جديد</h3>
<p class="font-body text-on-surface-variant text-sm px-4">قم بتوصيل صندوق نديم جديد أو أي ملحق آخر لتوسيع تجربة اللعب والتعلم.</p>
</div>
</div>
</div>
</div>
</main>
<!-- Bottom Nav Bar Mobile Placeholder (For conceptual completion based on rules) -->
<nav class="md:hidden fixed bottom-0 w-full bg-[#fafaf5] dark:bg-slate-900 border-t border-slate-100 dark:border-slate-800 z-50 flex justify-around py-3 pb-safe shadow-[0_-4px_20px_rgba(0,0,0,0.05)]">
<a class="flex flex-col items-center gap-1 text-slate-500" href="#">
<span class="material-symbols-outlined" style="font-variation-settings: 'FILL' 0;">home</span>
<span class="text-[10px] font-['Cairo'] font-medium">الرئيسية</span>
</a>
<a class="flex flex-col items-center gap-1 text-primary" href="#">
<div class="bg-primary/10 p-1.5 rounded-full">
<span class="material-symbols-outlined" style="font-variation-settings: 'FILL' 1;">devices</span>
</div>
<span class="text-[10px] font-['Cairo'] font-bold">أجهزتي</span>
</a>
<a class="flex flex-col items-center gap-1 text-slate-500" href="#">
<span class="material-symbols-outlined" style="font-variation-settings: 'FILL' 0;">library_music</span>
<span class="text-[10px] font-['Cairo'] font-medium">المكتبة</span>
</a>
<a class="flex flex-col items-center gap-1 text-slate-500" href="#">
<span class="material-symbols-outlined" style="font-variation-settings: 'FILL' 0;">settings</span>
<span class="text-[10px] font-['Cairo'] font-medium">الإعدادات</span>
</a>
</nav>
</body></html>

<!-- Semsems Library -->
<!DOCTYPE html>

<html dir="rtl" lang="ar"><head>
<meta charset="utf-8"/>
<meta content="width=device-width, initial-scale=1.0" name="viewport"/>
<title>مكتبة السماسم - نديم</title>
<script src="https://cdn.tailwindcss.com?plugins=forms,container-queries"></script>
<link href="https://fonts.googleapis.com/css2?family=Baloo+Bhaijaan+2:wght@400;500;600;700;800&amp;family=Cairo:wght@400;500;600;700&amp;display=swap" rel="stylesheet"/>
<link href="https://fonts.googleapis.com/css2?family=Material+Symbols+Outlined:wght,FILL@100..700,0..1&amp;display=swap" rel="stylesheet"/>
<link href="https://fonts.googleapis.com/css2?family=Material+Symbols+Outlined:wght,FILL@100..700,0..1&amp;display=swap" rel="stylesheet"/>
<script id="tailwind-config">
        tailwind.config = {
            darkMode: "class",
            theme: {
                extend: {
                    "colors": {
                        "tertiary-fixed-dim": "#d6c953",
                        "inverse-primary": "#7cd0ff",
                        "on-primary-container": "#00455f",
                        "tertiary-container": "#baae3b",
                        "on-background": "#1a1c19",
                        "on-surface-variant": "#3e484f",
                        "secondary-container": "#fdaa3f",
                        "surface-dim": "#dadad5",
                        "on-primary-fixed": "#001e2c",
                        "secondary-fixed": "#ffddba",
                        "surface": "#fafaf5",
                        "on-primary": "#ffffff",
                        "surface-container-high": "#e8e8e3",
                        "surface-bright": "#fafaf5",
                        "inverse-surface": "#2f312e",
                        "on-tertiary-fixed-variant": "#4e4800",
                        "primary-fixed-dim": "#7cd0ff",
                        "on-primary-fixed-variant": "#004c69",
                        "error-container": "#ffdad6",
                        "outline-variant": "#bec8d0",
                        "secondary": "#875200",
                        "primary": "#00668a",
                        "on-secondary": "#ffffff",
                        "tertiary": "#686000",
                        "on-secondary-fixed-variant": "#673d00",
                        "on-error-container": "#93000a",
                        "secondary-fixed-dim": "#ffb866",
                        "surface-container-low": "#f4f4ef",
                        "surface-container-highest": "#e3e3de",
                        "surface-container": "#eeeee9",
                        "tertiary-fixed": "#f3e56c",
                        "on-secondary-container": "#6d4100",
                        "outline": "#6f7880",
                        "on-tertiary-fixed": "#1f1c00",
                        "on-secondary-fixed": "#2b1700",
                        "on-tertiary-container": "#474100",
                        "primary-container": "#4fb6e9",
                        "primary-fixed": "#c4e7ff",
                        "on-error": "#ffffff",
                        "on-surface": "#1a1c19",
                        "surface-variant": "#e3e3de",
                        "on-tertiary": "#ffffff",
                        "surface-tint": "#00668a",
                        "background": "#fafaf5",
                        "surface-container-lowest": "#ffffff",
                        "inverse-on-surface": "#f1f1ec",
                        "error": "#ba1a1a"
                    },
                    "borderRadius": {
                        "DEFAULT": "1rem",
                        "lg": "2rem",
                        "xl": "3rem",
                        "full": "9999px"
                    },
                    "fontFamily": {
                        "headline": ["'Baloo Bhaijaan 2'", "sans-serif"],
                        "body": ["'Cairo'", "sans-serif"],
                        "label": ["'Cairo'", "sans-serif"]
                    }
                }
            }
        }
    </script>
<style>
        body {
            font-family: 'Cairo', sans-serif;
        }
        h1, h2, h3, h4, h5, h6, .font-headline {
            font-family: 'Baloo Bhaijaan 2', sans-serif;
        }
    </style>
</head>
<body class="bg-background text-on-background min-h-screen flex selection:bg-primary-container selection:text-on-primary-container">
<!-- Desktop Sidebar -->
<nav class="hidden md:flex fixed right-0 top-0 h-full w-72 bg-slate-50 dark:bg-slate-900 border-l-0 rounded-l-[3rem] z-40 flex-col p-6 space-y-8 bg-surface-container-low shadow-[-4px_0_24px_rgba(0,0,0,0.02)] rtl">
<div class="flex items-center space-x-4 space-x-reverse mb-8"><div class="w-full flex justify-center"><img alt="نديم شعار" class="h-24 w-auto object-contain" src="https://lh3.googleusercontent.com/aida/ADBb0uhb3K05xBavCVb8YCsNHNxC0dDQA0iQEA1KfFM9iICU-JcTNcetpLmTTfmqcDUgR8rweEGZL_FhQHSM5syLac5m6Ur3xoSv8bP8jNyNT5PG4r0SHMsHsHp8fh68Nu-ELeKvlmkBkY7adaDzAwYGW4VhS2ExRn38Brsa2cyqIVEo7Yt6ri4Hczz1BjVyJDShwPyhot3-F3RQgkecwwh9sflPZgczN1ZN4ASR6uCQWS4RUe4my1mkf3uxlZ-wNyheZLhPixhadMOUf4c"/></div></div>
<div class="flex flex-col space-y-2 flex-grow">
<!-- Inactive -->
<a class="flex items-center space-x-3 space-x-reverse px-4 py-3 rounded-full text-slate-600 dark:text-slate-400 hover:text-[#00668a] hover:bg-slate-100 dark:hover:bg-slate-800 transition-all duration-300" href="#">
<span class="material-symbols-outlined">home</span>
<span class="font-['Cairo'] text-sm font-medium tracking-wide">الرئيسية</span>
</a>
<!-- Inactive -->
<a class="flex items-center space-x-3 space-x-reverse px-4 py-3 rounded-full text-slate-600 dark:text-slate-400 hover:text-[#00668a] hover:bg-slate-100 dark:hover:bg-slate-800 transition-all duration-300" href="#">
<span class="material-symbols-outlined">devices</span>
<span class="font-['Cairo'] text-sm font-medium tracking-wide">أجهزتي</span>
</a>
<!-- Active -->
<a class="flex items-center space-x-3 space-x-reverse px-4 py-3 bg-gradient-to-br from-[#00668a] to-[#4fb6e9] text-white rounded-full shadow-lg scale-105 active:scale-95 transition-transform ease-out" href="#">
<span class="material-symbols-outlined" style="font-variation-settings: 'FILL' 1;">library_music</span>
<span class="font-['Cairo'] text-sm font-medium tracking-wide font-bold">مكتبة السماسم</span>
</a>
<!-- Inactive -->
<a class="flex items-center space-x-3 space-x-reverse px-4 py-3 rounded-full text-slate-600 dark:text-slate-400 hover:text-[#00668a] hover:bg-slate-100 dark:hover:bg-slate-800 transition-all duration-300" href="#">
<span class="material-symbols-outlined">forum</span>
<span class="font-['Cairo'] text-sm font-medium tracking-wide">سجل المحادثات الذكية</span>
</a>
<!-- Inactive -->
<a class="flex items-center space-x-3 space-x-reverse px-4 py-3 rounded-full text-slate-600 dark:text-slate-400 hover:text-[#00668a] hover:bg-slate-100 dark:hover:bg-slate-800 transition-all duration-300" href="#">
<span class="material-symbols-outlined">settings</span>
<span class="font-['Cairo'] text-sm font-medium tracking-wide">الإعدادات</span>
</a>
</div>
</nav>
<!-- Mobile Top App Bar -->
<header class="md:hidden fixed top-0 left-0 right-0 z-30 flex justify-between items-center px-6 h-20 bg-[#fafaf5]/80 dark:bg-slate-950/80 backdrop-blur-xl border-b-0 shadow-sm rtl">
<div class="flex items-center space-x-4 space-x-reverse"><img alt="نديم شعار" class="h-12 w-auto object-contain" src="https://lh3.googleusercontent.com/aida/ADBb0uhb3K05xBavCVb8YCsNHNxC0dDQA0iQEA1KfFM9iICU-JcTNcetpLmTTfmqcDUgR8rweEGZL_FhQHSM5syLac5m6Ur3xoSv8bP8jNyNT5PG4r0SHMsHsHp8fh68Nu-ELeKvlmkBkY7adaDzAwYGW4VhS2ExRn38Brsa2cyqIVEo7Yt6ri4Hczz1BjVyJDShwPyhot3-F3RQgkecwwh9sflPZgczN1ZN4ASR6uCQWS4RUe4my1mkf3uxlZ-wNyheZLhPixhadMOUf4c"/></div>
<div class="flex items-center space-x-4 space-x-reverse">
<button class="text-[#00668a] dark:text-[#4fb6e9] hover:opacity-80 transition-opacity">
<span class="material-symbols-outlined">notifications</span>
</button>
<button class="text-[#00668a] dark:text-[#4fb6e9] hover:opacity-80 transition-opacity">
<span class="material-symbols-outlined">child_care</span>
</button>
</div>
</header>
<!-- Main Content Area -->
<main class="flex-1 md:mr-72 pt-24 md:pt-12 px-6 pb-24 md:pb-12 max-w-7xl mx-auto w-full">
<!-- Header Section -->
<div class="mb-12 flex justify-between items-end">
<div>
<h1 class="font-headline text-5xl font-extrabold text-primary mb-2 tracking-wide">مكتبة السماسم</h1>
<p class="text-on-surface-variant text-lg max-w-xl leading-relaxed">استكشف مجموعة متنوعة من السماسم المصممة لإثراء خيال طفلك. اختر بين القصص العادية والتفاعلات الذكية.</p>
</div>
<div class="hidden md:flex space-x-3 space-x-reverse bg-surface-container-low p-1 rounded-full">
<button class="px-6 py-2 bg-primary text-on-primary rounded-full font-bold shadow-sm transition-all hover:scale-[1.02]">الكل</button>
<button class="px-6 py-2 text-on-surface-variant rounded-full hover:bg-surface-container-high transition-colors">عادي</button>
<button class="px-6 py-2 text-on-surface-variant rounded-full hover:bg-surface-container-high transition-colors">ذكي</button>
</div>
</div>
<!-- Library Grid -->
<div class="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-3 xl:grid-cols-4 gap-8">
<!-- Card 1: Normal Audio -->
<div class="group relative bg-surface-container-highest rounded-[2rem] overflow-hidden hover:scale-[1.02] hover:bg-surface-bright transition-all duration-300 cursor-pointer flex flex-col h-full">
<div class="relative h-48 w-full bg-surface-container overflow-hidden">
<img alt="مجموعة الحيوانات" class="w-full h-full object-cover group-hover:scale-105 transition-transform duration-500" data-alt="Collection of cute wooden animal figurines on a soft pastel background, well lit, playful aesthetic" src="https://lh3.googleusercontent.com/aida-public/AB6AXuD0y9OyINTIrxzGY4uYJG-1lhuhXwl6nVROrj-el0Z0p9DhVdjRN5_lT_S0u00dHPzeaqWSi7dyFEHNIiSvf2Gfe_-RN9e4UktDLtfIVvfxkhkM7DSVMJLNEf6iES6R2K4td4GGWRHWr_GDR62IMy0FqNAV3z4Y-6IiCQ8dsoc4Zfz1leC6nqSHBp9NAGM8rFZbti1ICAjgKjtLsgtCHk2K__zVZCbDK6KhDYyql2_Stso9Fl5EvlyVZyV9yiZqYzPVN3MR7jwaUGF1"/>
<div class="absolute top-4 right-4 bg-surface/90 backdrop-blur-md px-3 py-1 rounded-full flex items-center space-x-1 space-x-reverse shadow-sm">
<span class="material-symbols-outlined text-primary text-sm" style="font-variation-settings: 'FILL' 1;">volume_up</span>
<span class="text-sm font-bold text-primary">عادي</span>
</div>
</div>
<div class="p-6 flex flex-col flex-grow">
<h3 class="font-headline text-2xl font-bold text-on-surface mb-2">مجموعة الحيوانات</h3>
<p class="text-sm text-on-surface-variant flex-grow">قصص صوتية ممتعة عن حيوانات الغابة والمزرعة.</p>
<div class="mt-4 flex items-center justify-between">
<span class="text-xs text-outline font-medium">٤٥ دقيقة</span>
<button class="w-10 h-10 rounded-full bg-primary/10 text-primary flex items-center justify-center group-hover:bg-primary group-hover:text-on-primary transition-colors">
<span class="material-symbols-outlined">play_arrow</span>
</button>
</div>
</div>
</div>
<!-- Card 2: Pro AI -->
<div class="group relative bg-surface-container-highest rounded-[2rem] overflow-hidden hover:scale-[1.02] hover:bg-surface-bright transition-all duration-300 cursor-pointer flex flex-col h-full ring-2 ring-primary/20">
<div class="relative h-48 w-full bg-surface-container overflow-hidden">
<div class="absolute inset-0 bg-gradient-to-tr from-primary/20 to-transparent mix-blend-overlay z-10"></div>
<img alt="المهندس أحمد" class="w-full h-full object-cover group-hover:scale-105 transition-transform duration-500" data-alt="A friendly looking toy engineer figure standing in front of colorful building blocks, soft studio lighting" src="https://lh3.googleusercontent.com/aida-public/AB6AXuDxu9ZHcVklonrQIiQ9VziR2escVwU4r_99edL3nggx7t10AVrZa6SOdQrNJiZMh-BGam9UE-3I_GaerJqkCf22wKAbvsTGv3mjmtil_eEqgCktmfhvlD75tSdOSQ2TfYwCYxvP24aUmvhrh9IGEFMe6Nz6iSBkauayRRah3fUn_I4Uhu_xNx2_amquU8kzl0AvCnMlr8nyoqcyLfPEO271FAnrh98IxJ9LeDcGf1ImKIpKSgTgmm7DfwF4Xrry0F4XxrD9x342d3VI"/>
<div class="absolute top-4 right-4 bg-tertiary-container text-on-tertiary-container px-3 py-1 rounded-full flex items-center space-x-1 space-x-reverse shadow-sm z-20">
<span class="material-symbols-outlined text-sm" style="font-variation-settings: 'FILL' 1;">smart_toy</span>
<span class="text-sm font-bold">ذكي</span>
</div>
</div>
<div class="p-6 flex flex-col flex-grow">
<h3 class="font-headline text-2xl font-bold text-on-surface mb-2">المهندس أحمد</h3>
<p class="text-sm text-on-surface-variant flex-grow">شخصية تفاعلية تجيب على أسئلة طفلك حول البناء والاختراعات.</p>
<div class="mt-4 flex items-center justify-between">
<span class="text-xs text-outline font-medium">محادثة مفتوحة</span>
<button class="w-10 h-10 rounded-full bg-gradient-to-br from-primary to-primary-container text-on-primary flex items-center justify-center shadow-md group-hover:shadow-lg transition-all">
<span class="material-symbols-outlined">forum</span>
</button>
</div>
</div>
</div>
<!-- Card 3: Normal Audio -->
<div class="group relative bg-surface-container-highest rounded-[2rem] overflow-hidden hover:scale-[1.02] hover:bg-surface-bright transition-all duration-300 cursor-pointer flex flex-col h-full">
<div class="relative h-48 w-full bg-surface-container overflow-hidden">
<img alt="حكايات الفضاء" class="w-full h-full object-cover group-hover:scale-105 transition-transform duration-500" data-alt="Toy astronaut figure floating against a backdrop of dark blue with painted stars and planets, magical mood" src="https://lh3.googleusercontent.com/aida-public/AB6AXuBD5wr4CvZ9qGbJ9NjHwSvhtLvjbNxE7KAJH_NxULGy_TO_5J25VA_2WYAr9ye8hpkPjLQwu6X_8enqW2B5OEhUZJsiDlU3Ca1M4vudVvAQbRKMLD_Ne6vFCa5aMqgAP09Sy1eQ_g93edfdG3vJrpFeTjKY40izb-Mod1eAsjUcHcNo9j0WOlqB_uSNMIC1BOYgcZP34fGeY6KS8YgrVVPuBNdqCWY9oE_ulSs3fZq_-gOaZStsC6oOAk8VPX3aUrqy1SK8zsmp6x2P"/>
<div class="absolute top-4 right-4 bg-surface/90 backdrop-blur-md px-3 py-1 rounded-full flex items-center space-x-1 space-x-reverse shadow-sm">
<span class="material-symbols-outlined text-primary text-sm" style="font-variation-settings: 'FILL' 1;">volume_up</span>
<span class="text-sm font-bold text-primary">عادي</span>
</div>
</div>
<div class="p-6 flex flex-col flex-grow">
<h3 class="font-headline text-2xl font-bold text-on-surface mb-2">حكايات الفضاء</h3>
<p class="text-sm text-on-surface-variant flex-grow">رحلة صوتية هادئة لاستكشاف الكواكب والنجوم قبل النوم.</p>
<div class="mt-4 flex items-center justify-between">
<span class="text-xs text-outline font-medium">٣٠ دقيقة</span>
<button class="w-10 h-10 rounded-full bg-primary/10 text-primary flex items-center justify-center group-hover:bg-primary group-hover:text-on-primary transition-colors">
<span class="material-symbols-outlined">play_arrow</span>
</button>
</div>
</div>
</div>
<!-- Card 4: Pro AI -->
<div class="group relative bg-surface-container-highest rounded-[2rem] overflow-hidden hover:scale-[1.02] hover:bg-surface-bright transition-all duration-300 cursor-pointer flex flex-col h-full ring-2 ring-primary/20">
<div class="relative h-48 w-full bg-surface-container overflow-hidden">
<div class="absolute inset-0 bg-gradient-to-tr from-primary/20 to-transparent mix-blend-overlay z-10"></div>
<img alt="المعلمة سارة" class="w-full h-full object-cover group-hover:scale-105 transition-transform duration-500" data-alt="A cute toy teacher figurine holding a tiny book, sitting on a miniature desk, warm natural light" src="https://lh3.googleusercontent.com/aida-public/AB6AXuBAkFZwf5G5Zw2p4_hGLuGHD55zx-fWoCp9-Juz7rF2SjvhKmGpo9vCKHR--2f3SI8QSTvqdjC7tX5dmnZT6SsSlHr11I9o1RWbVDmcHhHm6CrkjMN2hK_gaUtgYszZ1rUlCXmScihYDiXJzJ7tJ9AGfiYd5mBYaNNFKvl0-5M1L3NsBrGbqWsmiZVErq9Zro_bCXiCfLktgPkG130ddz6zRKTazF1twfq2b9QmksiC06MUZatFqPqT6ciPcnm5BkgDo8lTaxp4svZn"/>
<div class="absolute top-4 right-4 bg-tertiary-container text-on-tertiary-container px-3 py-1 rounded-full flex items-center space-x-1 space-x-reverse shadow-sm z-20">
<span class="material-symbols-outlined text-sm" style="font-variation-settings: 'FILL' 1;">smart_toy</span>
<span class="text-sm font-bold">ذكي</span>
</div>
</div>
<div class="p-6 flex flex-col flex-grow">
<h3 class="font-headline text-2xl font-bold text-on-surface mb-2">المعلمة سارة</h3>
<p class="text-sm text-on-surface-variant flex-grow">رفيقة ذكية تساعد في تعلم الحروف والأرقام بطريقة تفاعلية.</p>
<div class="mt-4 flex items-center justify-between">
<span class="text-xs text-outline font-medium">محادثة تعليمية</span>
<button class="w-10 h-10 rounded-full bg-gradient-to-br from-primary to-primary-container text-on-primary flex items-center justify-center shadow-md group-hover:shadow-lg transition-all">
<span class="material-symbols-outlined">forum</span>
</button>
</div>
</div>
</div>
</div>
</main>
<!-- Mobile Bottom Nav (Suppressed as per rules since this is a top level destination but we have sidebar/topbar logic. Wait, rules say "On MOBILE, prohibit top-row text links and use BottomNavBar". Let's add a basic mobile bottom nav) -->
<nav class="md:hidden fixed bottom-0 left-0 right-0 bg-surface/90 backdrop-blur-xl border-t-0 shadow-[0_-4px_24px_rgba(0,0,0,0.05)] z-40 pb-safe rtl">
<div class="flex justify-around items-center h-20 px-2">
<!-- Inactive -->
<a class="flex flex-col items-center justify-center w-full h-full text-slate-500 hover:text-primary transition-colors" href="#">
<span class="material-symbols-outlined mb-1">home</span>
<span class="text-[0.6875rem] font-medium">الرئيسية</span>
</a>
<!-- Active -->
<a class="flex flex-col items-center justify-center w-full h-full text-primary font-bold" href="#">
<div class="bg-primary-container/30 px-5 py-1 rounded-full mb-1">
<span class="material-symbols-outlined" style="font-variation-settings: 'FILL' 1;">library_music</span>
</div>
<span class="text-[0.6875rem]">مكتبة السماسم</span>
</a>
<!-- Inactive -->
<a class="flex flex-col items-center justify-center w-full h-full text-slate-500 hover:text-primary transition-colors" href="#">
<span class="material-symbols-outlined mb-1">forum</span>
<span class="text-[0.6875rem] font-medium">سجل المحادثات</span>
</a>
<!-- Inactive -->
<a class="flex flex-col items-center justify-center w-full h-full text-slate-500 hover:text-primary transition-colors" href="#">
<span class="material-symbols-outlined mb-1">settings</span>
<span class="text-[0.6875rem] font-medium">الإعدادات</span>
</a>
</div>
</nav>
</body></html>

<!-- Semsem Details -->
<!DOCTYPE html>

<html class="light" dir="rtl" lang="ar"><head>
<meta charset="utf-8"/>
<meta content="width=device-width, initial-scale=1.0" name="viewport"/>
<title>تفاصيل السمسم - نديم</title>
<script src="https://cdn.tailwindcss.com?plugins=forms,container-queries"></script>
<link href="https://fonts.googleapis.com/css2?family=Baloo+Bhaijaan+2:wght@400;500;600;700;800&amp;family=Cairo:wght@400;500;600;700&amp;display=swap" rel="stylesheet"/>
<link href="https://fonts.googleapis.com/css2?family=Material+Symbols+Outlined:wght,FILL@100..700,0..1&amp;display=swap" rel="stylesheet"/>
<link href="https://fonts.googleapis.com/css2?family=Material+Symbols+Outlined:wght,FILL@100..700,0..1&amp;display=swap" rel="stylesheet"/>
<script id="tailwind-config">
        tailwind.config = {
            darkMode: "class",
            theme: {
                extend: {
                    "colors": {
                        "tertiary-fixed-dim": "#d6c953",
                        "inverse-primary": "#7cd0ff",
                        "on-primary-container": "#00455f",
                        "tertiary-container": "#baae3b",
                        "on-background": "#1a1c19",
                        "on-surface-variant": "#3e484f",
                        "secondary-container": "#fdaa3f",
                        "surface-dim": "#dadad5",
                        "on-primary-fixed": "#001e2c",
                        "secondary-fixed": "#ffddba",
                        "surface": "#fafaf5",
                        "on-primary": "#ffffff",
                        "surface-container-high": "#e8e8e3",
                        "surface-bright": "#fafaf5",
                        "inverse-surface": "#2f312e",
                        "on-tertiary-fixed-variant": "#4e4800",
                        "primary-fixed-dim": "#7cd0ff",
                        "on-primary-fixed-variant": "#004c69",
                        "error-container": "#ffdad6",
                        "outline-variant": "#bec8d0",
                        "secondary": "#875200",
                        "primary": "#00668a",
                        "on-secondary": "#ffffff",
                        "tertiary": "#686000",
                        "on-secondary-fixed-variant": "#673d00",
                        "on-error-container": "#93000a",
                        "secondary-fixed-dim": "#ffb866",
                        "surface-container-low": "#f4f4ef",
                        "surface-container-highest": "#e3e3de",
                        "surface-container": "#eeeee9",
                        "tertiary-fixed": "#f3e56c",
                        "on-secondary-container": "#6d4100",
                        "outline": "#6f7880",
                        "on-tertiary-fixed": "#1f1c00",
                        "on-secondary-fixed": "#2b1700",
                        "on-tertiary-container": "#474100",
                        "primary-container": "#4fb6e9",
                        "primary-fixed": "#c4e7ff",
                        "on-error": "#ffffff",
                        "on-surface": "#1a1c19",
                        "surface-variant": "#e3e3de",
                        "on-tertiary": "#ffffff",
                        "surface-tint": "#00668a",
                        "background": "#fafaf5",
                        "surface-container-lowest": "#ffffff",
                        "inverse-on-surface": "#f1f1ec",
                        "error": "#ba1a1a"
                    },
                    "borderRadius": {
                        "DEFAULT": "1rem",
                        "lg": "2rem",
                        "xl": "3rem",
                        "full": "9999px"
                    },
                    "fontFamily": {
                        "headline": ["'Baloo Bhaijaan 2'", "sans-serif"],
                        "body": ["'Cairo'", "sans-serif"],
                        "label": ["'Cairo'", "sans-serif"]
                    }
                }
            }
        }
    </script>
<style>
        body {
            font-family: 'Cairo', sans-serif;
            background-color: theme('colors.background');
            color: theme('colors.on-surface');
            overflow-x: hidden;
        }
        h1, h2, h3, h4, h5, h6, .font-headline {
            font-family: 'Baloo Bhaijaan 2', sans-serif;
        }
        .material-symbols-outlined {
            font-variation-settings: 'FILL' 0, 'wght' 400, 'GRAD' 0, 'opsz' 24;
        }
        .material-symbols-outlined.fill {
            font-variation-settings: 'FILL' 1;
        }
        
        /* Subtle Texture Overlay */
        .texture-overlay::before {
            content: "";
            position: absolute;
            top: 0; left: 0; right: 0; bottom: 0;
            background-image: url("data:image/svg+xml,%3Csvg viewBox='0 0 200 200' xmlns='http://www.w3.org/2000/svg'%3E%3Cfilter id='noiseFilter'%3E%3CfeTurbulence type='fractalNoise' baseFrequency='0.85' numOctaves='3' stitchTiles='stitch'/%3E%3C/filter%3E%3Crect width='100%25' height='100%25' filter='url(%23noiseFilter)' opacity='0.02'/%3E%3C/svg%3E");
            pointer-events: none;
            border-radius: inherit;
        }
    </style>
</head>
<body class="bg-background min-h-screen text-on-surface flex rtl relative">
<!-- TopNavBar (Web/Tablet) -->
<nav class="hidden md:flex fixed top-0 left-0 right-0 z-30 justify-between items-center px-12 h-20 bg-[#fafaf5]/80 dark:bg-slate-950/80 backdrop-blur-xl transition-opacity hover:opacity-80 rtl" style="box-shadow: 0 4px 30px rgba(0, 0, 0, 0.05);">
<div class="flex items-center gap-4"><img alt="نديم" class="h-12 w-auto object-contain" src="https://lh3.googleusercontent.com/aida/ADBb0uhb3K05xBavCVb8YCsNHNxC0dDQA0iQEA1KfFM9iICU-JcTNcetpLmTTfmqcDUgR8rweEGZL_FhQHSM5syLac5m6Ur3xoSv8bP8jNyNT5PG4r0SHMsHsHp8fh68Nu-ELeKvlmkBkY7adaDzAwYGW4VhS2ExRn38Brsa2cyqIVEo7Yt6ri4Hczz1BjVyJDShwPyhot3-F3RQgkecwwh9sflPZgczN1ZN4ASR6uCQWS4RUe4my1mkf3uxlZ-wNyheZLhPixhadMOUf4c"/></div>
<div class="flex items-center gap-6">
<button class="text-[#00668a] hover:bg-surface-container p-2 rounded-full transition-colors relative">
<span class="material-symbols-outlined text-2xl">notifications</span>
<span class="absolute top-2 right-2 w-2 h-2 bg-secondary-container rounded-full"></span>
</button>
<button class="text-[#00668a] hover:bg-surface-container p-2 rounded-full transition-colors">
<span class="material-symbols-outlined text-2xl">child_care</span>
</button>
<div class="w-10 h-10 rounded-full overflow-hidden border-2 border-primary-container p-0.5">
<img alt="صورة المستخدم" class="w-full h-full object-cover rounded-full" data-alt="Portrait of a smiling user in soft natural light, warm tone" src="https://lh3.googleusercontent.com/aida-public/AB6AXuBs8vCSuJ87wvNOCWhuqoyw2JGRxD5g4xLgqk7kOYbCWnksU4zGo7JdAgp4887va25pRsynFE4_b9innIEOCN17_c7IRrbP-RtPhJhasJC9ysETy7qHW5gYDT8FqhGRufwQrf1rRo2Q6TGjwsghdUR_IGVNyAA0IKz4HKv95kV-z2xYxZloJd757AGneNZZZXNCCCD_WfDNPDo5pO7RuBlNvLz_WKyKi-mM9flLpektHc4PXW39jwlT1kMgw3Hib-iq-PdNKpY14UEF"/>
</div>
</div>
</nav>
<!-- SideNavBar (Drawer - Fixed Left for LTR, Right for RTL in code, visually left due to rtl dir) -->
<aside class="hidden md:flex fixed right-0 top-0 h-full z-40 flex-col p-6 space-y-8 rtl docked w-72 border-l-0 rounded-l-[3rem] bg-slate-50 dark:bg-slate-900 bg-surface-container-low shadow-[-4px_0_24px_rgba(0,0,0,0.02)]">
<!-- Logo Area -->
<div class="flex items-center gap-4 mb-8 mt-20 px-2">
<img alt="نديم" class="h-20 w-auto object-contain" src="https://lh3.googleusercontent.com/aida/ADBb0uhb3K05xBavCVb8YCsNHNxC0dDQA0iQEA1KfFM9iICU-JcTNcetpLmTTfmqcDUgR8rweEGZL_FhQHSM5syLac5m6Ur3xoSv8bP8jNyNT5PG4r0SHMsHsHp8fh68Nu-ELeKvlmkBkY7adaDzAwYGW4VhS2ExRn38Brsa2cyqIVEo7Yt6ri4Hczz1BjVyJDShwPyhot3-F3RQgkecwwh9sflPZgczN1ZN4ASR6uCQWS4RUe4my1mkf3uxlZ-wNyheZLhPixhadMOUf4c"/>
</div>
<nav class="flex-1 space-y-2">
<a class="flex items-center gap-4 px-4 py-3 font-['Cairo'] text-sm font-medium tracking-wide text-slate-600 dark:text-slate-400 hover:text-[#00668a] hover:bg-slate-100 dark:hover:bg-slate-800 transition-all duration-300 rounded-xl" href="#">
<span class="material-symbols-outlined">home</span>
                الرئيسية
            </a>
<a class="flex items-center gap-4 px-4 py-3 font-['Cairo'] text-sm font-medium tracking-wide text-slate-600 dark:text-slate-400 hover:text-[#00668a] hover:bg-slate-100 dark:hover:bg-slate-800 transition-all duration-300 rounded-xl" href="#">
<span class="material-symbols-outlined">devices</span>
                أجهزتي
            </a>
<!-- Active State: Library matches intent of viewing details of a Semsem (content item) -->
<a class="flex items-center gap-4 px-4 py-3 font-['Cairo'] text-sm font-medium tracking-wide bg-gradient-to-br from-[#00668a] to-[#4fb6e9] text-white rounded-full shadow-lg scale-105 transition-transform ease-out" href="#">
<span class="material-symbols-outlined fill">library_music</span>
                مكتبة السماسم
            </a>
<a class="flex items-center gap-4 px-4 py-3 font-['Cairo'] text-sm font-medium tracking-wide text-slate-600 dark:text-slate-400 hover:text-[#00668a] hover:bg-slate-100 dark:hover:bg-slate-800 transition-all duration-300 rounded-xl" href="#">
<span class="material-symbols-outlined">forum</span>
                سجل المحادثات الذكية
            </a>
<a class="flex items-center gap-4 px-4 py-3 font-['Cairo'] text-sm font-medium tracking-wide text-slate-600 dark:text-slate-400 hover:text-[#00668a] hover:bg-slate-100 dark:hover:bg-slate-800 transition-all duration-300 rounded-xl mt-auto" href="#">
<span class="material-symbols-outlined">settings</span>
                الإعدادات
            </a>
</nav>
</aside>
<!-- Main Content Canvas -->
<main class="flex-1 w-full md:pr-72 pt-24 pb-24 md:pb-12 px-4 md:px-12 transition-all duration-300">
<!-- Breadcrumb & Back -->
<div class="flex items-center gap-3 mb-8 text-on-surface-variant">
<button class="w-10 h-10 rounded-full bg-surface-container-highest flex items-center justify-center hover:bg-surface-variant transition-colors group">
<span class="material-symbols-outlined text-on-surface-variant group-hover:text-primary transition-colors">arrow_forward</span> <!-- RTL arrow -->
</button>
<div class="flex items-center gap-2 font-body text-sm">
<span class="hover:text-primary cursor-pointer transition-colors">مكتبة السماسم</span>
<span class="material-symbols-outlined text-sm">chevron_left</span>
<span class="font-bold text-primary">تفاصيل السمسم</span>
</div>
</div>
<!-- Details Header Card (Pro/Smart Focus) -->
<section class="mb-12 relative rounded-xl bg-surface-container-highest p-8 md:p-12 overflow-hidden flex flex-col md:flex-row gap-8 items-center shadow-[0_10px_40px_-10px_rgba(0,102,138,0.08)] texture-overlay">
<!-- Abstract decorative shape -->
<div class="absolute -top-24 -right-24 w-64 h-64 bg-primary-container/20 rounded-full blur-3xl"></div>
<div class="absolute -bottom-24 -left-24 w-80 h-80 bg-secondary-container/10 rounded-full blur-3xl"></div>
<!-- Semsem Avatar/Icon -->
<div class="relative z-10 w-40 h-40 md:w-56 md:h-56 shrink-0 rounded-[3rem] bg-surface flex items-center justify-center shadow-lg border-4 border-surface p-2">
<div class="w-full h-full rounded-[2.5rem] bg-gradient-to-br from-tertiary-container to-tertiary-fixed-dim flex items-center justify-center overflow-hidden relative">
<span class="material-symbols-outlined text-6xl text-on-tertiary-container z-10">auto_awesome</span>
<!-- Simulated internal glow -->
<div class="absolute inset-0 bg-white/20 rounded-full blur-xl animate-pulse"></div>
</div>
<!-- Badge -->
<div class="absolute -bottom-4 left-1/2 -translate-x-1/2 bg-surface-container-lowest px-4 py-1.5 rounded-full shadow-md flex items-center gap-1">
<span class="w-2 h-2 rounded-full bg-primary animate-pulse"></span>
<span class="font-body text-xs font-bold text-primary">متصل</span>
</div>
</div>
<!-- Info Container -->
<div class="relative z-10 flex-1 space-y-4 text-center md:text-right">
<div class="inline-flex items-center gap-2 bg-primary/10 text-primary px-3 py-1 rounded-full font-body text-sm font-bold mb-2">
<span class="material-symbols-outlined text-sm">psychology</span>
                    سمسم ذكي (Pro)
                </div>
<h1 class="font-headline text-4xl md:text-5xl font-bold text-on-surface tracking-wide">الحكيم طارق</h1>
<p class="font-headline text-xl text-primary font-medium">راوي قصص تفاعلي</p>
<p class="font-body text-on-surface-variant leading-relaxed max-w-2xl mt-4">
                    طارق هو سمسم ذكي مصمم لنسج حكايات خيالية تتفاعل مع اختيارات طفلك. يستخدم الذكاء الاصطناعي لإنشاء نهايات متعددة وبناء عوالم ساحرة تعزز الإبداع والخيال.
                </p>
<div class="flex flex-wrap items-center gap-4 mt-6 justify-center md:justify-start">
<button class="bg-gradient-to-br from-primary to-primary-container text-white rounded-full px-8 py-3.5 font-headline font-bold text-lg shadow-[0_8px_20px_-6px_rgba(0,102,138,0.4)] hover:scale-105 transition-transform flex items-center gap-2">
<span class="material-symbols-outlined fill">play_arrow</span>
                        بدء محادثة
                    </button>
<button class="bg-surface-container-lowest text-primary border border-outline-variant/30 rounded-full px-6 py-3.5 font-headline font-bold hover:bg-surface-container-high transition-colors flex items-center gap-2 shadow-sm">
<span class="material-symbols-outlined">settings_voice</span>
                        إعدادات الصوت
                    </button>
</div>
</div>
</section>
<!-- Bento Grid Layout for Secondary Info & Tracks -->
<div class="grid grid-cols-1 lg:grid-cols-3 gap-8">
<!-- Left Column: Normal Semsem Tracks (Takes 2 columns on large) -->
<div class="lg:col-span-2 space-y-6">
<div class="flex justify-between items-end mb-4">
<div>
<h2 class="font-headline text-2xl font-bold text-on-surface">المحتوى الصوتي</h2>
<p class="font-body text-sm text-on-surface-variant">المقاطع المتوفرة في السمسم العادي</p>
</div>
<span class="bg-surface-container-highest text-on-surface-variant px-3 py-1 rounded-lg font-body text-xs font-bold">5 مقاطع</span>
</div>
<!-- Custom Audio List (Avoiding standard lists) -->
<div class="space-y-4">
<!-- Track 1 -->
<div class="group bg-surface-container-lowest rounded-xl p-4 flex items-center gap-4 hover:bg-surface-container-high transition-colors cursor-pointer relative overflow-hidden shadow-sm hover:shadow-md">
<div class="w-12 h-12 rounded-full bg-secondary-container/20 flex items-center justify-center shrink-0 group-hover:bg-secondary-container/40 transition-colors">
<span class="material-symbols-outlined text-secondary group-hover:hidden">music_note</span>
<span class="material-symbols-outlined text-secondary hidden group-hover:block fill">play_arrow</span>
</div>
<div class="flex-1">
<h4 class="font-headline font-bold text-on-surface group-hover:text-primary transition-colors">قصة الفارس الشجاع</h4>
<p class="font-body text-xs text-on-surface-variant mt-0.5">12:45 • حكاية قبل النوم</p>
</div>
<button class="w-10 h-10 rounded-full text-outline hover:text-primary hover:bg-primary/10 flex items-center justify-center transition-colors">
<span class="material-symbols-outlined">more_vert</span>
</button>
</div>
<!-- Track 2 (Active/Playing state example) -->
<div class="group bg-primary-container/10 rounded-xl p-4 flex items-center gap-4 hover:bg-primary-container/20 transition-colors cursor-pointer relative overflow-hidden shadow-sm border border-primary/10">
<div class="absolute left-0 top-0 bottom-0 w-1 bg-primary"></div>
<div class="w-12 h-12 rounded-full bg-primary text-white flex items-center justify-center shrink-0 shadow-md">
<span class="material-symbols-outlined fill">pause</span>
</div>
<div class="flex-1">
<h4 class="font-headline font-bold text-primary">مغامرات في الغابة السحرية</h4>
<p class="font-body text-xs text-primary/70 mt-0.5">08:20 / 15:30 • مغامرة</p>
</div>
<!-- Mini visualizer -->
<div class="flex gap-1 items-end h-6 opacity-70">
<div class="w-1 bg-primary rounded-t-sm h-3 animate-pulse"></div>
<div class="w-1 bg-primary rounded-t-sm h-5 animate-pulse" style="animation-delay: 0.1s"></div>
<div class="w-1 bg-primary rounded-t-sm h-2 animate-pulse" style="animation-delay: 0.2s"></div>
<div class="w-1 bg-primary rounded-t-sm h-4 animate-pulse" style="animation-delay: 0.3s"></div>
</div>
<button class="w-10 h-10 rounded-full text-primary hover:bg-primary/20 flex items-center justify-center transition-colors ml-2">
<span class="material-symbols-outlined">more_vert</span>
</button>
</div>
<!-- Track 3 -->
<div class="group bg-surface-container-lowest rounded-xl p-4 flex items-center gap-4 hover:bg-surface-container-high transition-colors cursor-pointer relative overflow-hidden shadow-sm hover:shadow-md">
<div class="w-12 h-12 rounded-full bg-surface-container-highest flex items-center justify-center shrink-0 group-hover:bg-primary-container/20 transition-colors">
<span class="material-symbols-outlined text-outline group-hover:text-primary">music_note</span>
</div>
<div class="flex-1">
<h4 class="font-headline font-bold text-on-surface group-hover:text-primary transition-colors">أغنية الألوان</h4>
<p class="font-body text-xs text-on-surface-variant mt-0.5">03:15 • تعليمي</p>
</div>
<button class="w-10 h-10 rounded-full text-outline hover:text-primary hover:bg-primary/10 flex items-center justify-center transition-colors">
<span class="material-symbols-outlined">more_vert</span>
</button>
</div>
</div>
<!-- Add Content Button -->
<button class="w-full mt-4 py-4 rounded-xl border-2 border-dashed border-outline-variant/50 text-outline-variant hover:border-primary hover:text-primary hover:bg-primary/5 transition-all flex flex-col items-center justify-center gap-2 font-headline font-bold">
<span class="material-symbols-outlined text-2xl">add_circle</span>
                    إضافة محتوى جديد للسمسم العادي
                </button>
</div>
<!-- Right Column: Stats & Tags (Bento Style) -->
<div class="space-y-6">
<!-- Stat Card 1 -->
<div class="bg-surface-container-low rounded-xl p-6 flex flex-col justify-between h-40 relative overflow-hidden group hover:bg-surface-container transition-colors cursor-default">
<div class="absolute -left-6 -bottom-6 w-24 h-24 bg-primary/5 rounded-full group-hover:scale-150 transition-transform duration-500"></div>
<div class="flex justify-between items-start z-10">
<span class="font-body text-sm font-medium text-on-surface-variant">وقت الاستماع الإجمالي</span>
<span class="material-symbols-outlined text-primary bg-surface-container-highest p-1.5 rounded-lg">timer</span>
</div>
<div class="z-10">
<span class="font-headline text-3xl font-bold text-on-surface">14</span>
<span class="font-body text-sm text-on-surface-variant">ساعة و 30 دقيقة</span>
</div>
</div>
<!-- Tags / Themes Card -->
<div class="bg-surface-container-highest rounded-xl p-6">
<h3 class="font-headline text-lg font-bold text-on-surface mb-4 flex items-center gap-2">
<span class="material-symbols-outlined text-tertiary">category</span>
                        التصنيفات
                    </h3>
<div class="flex flex-wrap gap-2">
<span class="bg-surface px-3 py-1.5 rounded-lg font-body text-xs text-on-surface shadow-sm">خيال</span>
<span class="bg-surface px-3 py-1.5 rounded-lg font-body text-xs text-on-surface shadow-sm">مغامرة</span>
<span class="bg-surface px-3 py-1.5 rounded-lg font-body text-xs text-on-surface shadow-sm">تعليمي</span>
<span class="bg-surface px-3 py-1.5 rounded-lg font-body text-xs text-on-surface shadow-sm border border-dashed border-outline-variant text-outline cursor-pointer hover:border-primary hover:text-primary transition-colors">+ إضافة</span>
</div>
</div>
<!-- NFC Physical Status -->
<div class="bg-tertiary-container/10 rounded-xl p-6 border border-tertiary-container/20">
<h3 class="font-headline text-lg font-bold text-on-tertiary-container mb-2 flex items-center gap-2">
<span class="material-symbols-outlined">nfc</span>
                        حالة القطعة (NFC)
                    </h3>
<p class="font-body text-sm text-on-surface-variant mb-4">القطعة المادية مربوطة وجاهزة للاستخدام مع جهاز نديم.</p>
<div class="flex items-center gap-3">
<div class="relative w-8 h-8 flex items-center justify-center">
<span class="absolute inset-0 rounded-full bg-tertiary-container/40 animate-ping opacity-75"></span>
<div class="w-4 h-4 bg-tertiary-container rounded-full relative z-10"></div>
</div>
<span class="font-body text-xs font-bold text-tertiary">متصل بالشبكة</span>
</div>
</div>
</div>
</div>
</main>
<!-- BottomNavBar (Mobile) -->
<nav class="md:hidden fixed bottom-0 left-0 right-0 z-40 bg-surface-container-highest/90 backdrop-blur-md pb-safe border-t-0 rounded-t-[2rem] shadow-[0_-10px_40px_rgba(0,0,0,0.05)] rtl flex justify-around items-center px-2 py-3">
<a class="flex flex-col items-center gap-1 p-2 text-slate-500 hover:text-primary transition-colors w-16" href="#">
<span class="material-symbols-outlined text-[28px]">home</span>
<span class="font-body text-[10px] font-medium">الرئيسية</span>
</a>
<a class="flex flex-col items-center gap-1 p-2 text-slate-500 hover:text-primary transition-colors w-16" href="#">
<span class="material-symbols-outlined text-[28px]">devices</span>
<span class="font-body text-[10px] font-medium">أجهزتي</span>
</a>
<!-- Active Tab Mobile -->
<a class="flex flex-col items-center gap-1 p-2 text-primary font-bold w-16 relative -top-3" href="#">
<div class="bg-gradient-to-br from-primary to-primary-container text-white p-3 rounded-full shadow-lg mb-1">
<span class="material-symbols-outlined fill text-[24px]">library_music</span>
</div>
<span class="font-body text-[10px]">المكتبة</span>
</a>
<a class="flex flex-col items-center gap-1 p-2 text-slate-500 hover:text-primary transition-colors w-16" href="#">
<span class="material-symbols-outlined text-[28px]">forum</span>
<span class="font-body text-[10px] font-medium">السجل</span>
</a>
<a class="flex flex-col items-center gap-1 p-2 text-slate-500 hover:text-primary transition-colors w-16" href="#">
<span class="material-symbols-outlined text-[28px]">settings</span>
<span class="font-body text-[10px] font-medium">إعدادات</span>
</a>
</nav>
</body></html>

<!-- Design System -->
<!DOCTYPE html>

<html dir="rtl" lang="ar"><head>
<meta charset="utf-8"/>
<meta content="width=device-width, initial-scale=1.0" name="viewport"/>
<title>سجل المحادثات الذكية - نديم</title>
<!-- Fonts -->
<link href="https://fonts.googleapis.com" rel="preconnect"/>
<link crossorigin="" href="https://fonts.gstatic.com" rel="preconnect"/>
<link href="https://fonts.googleapis.com/css2?family=Baloo+Bhaijaan+2:wght@400;500;600;700;800&amp;family=Cairo:wght@400;500;600;700&amp;display=swap" rel="stylesheet"/>
<!-- Material Symbols -->
<link href="https://fonts.googleapis.com/css2?family=Material+Symbols+Outlined:wght,FILL@100..700,0..1&amp;display=swap" rel="stylesheet"/>
<link href="https://fonts.googleapis.com/css2?family=Material+Symbols+Outlined:wght,FILL@100..700,0..1&amp;display=swap" rel="stylesheet"/>
<!-- Tailwind CSS -->
<script src="https://cdn.tailwindcss.com?plugins=forms,container-queries"></script>
<!-- Tailwind Configuration -->
<script id="tailwind-config">
      tailwind.config = {
        darkMode: "class",
        theme: {
          extend: {
            colors: {
                "tertiary-fixed-dim": "#d6c953",
                "inverse-primary": "#7cd0ff",
                "on-primary-container": "#00455f",
                "tertiary-container": "#baae3b",
                "on-background": "#1a1c19",
                "on-surface-variant": "#3e484f",
                "secondary-container": "#fdaa3f",
                "surface-dim": "#dadad5",
                "on-primary-fixed": "#001e2c",
                "secondary-fixed": "#ffddba",
                "surface": "#fafaf5",
                "on-primary": "#ffffff",
                "surface-container-high": "#e8e8e3",
                "surface-bright": "#fafaf5",
                "inverse-surface": "#2f312e",
                "on-tertiary-fixed-variant": "#4e4800",
                "primary-fixed-dim": "#7cd0ff",
                "on-primary-fixed-variant": "#004c69",
                "error-container": "#ffdad6",
                "outline-variant": "#bec8d0",
                "secondary": "#875200",
                "primary": "#00668a",
                "on-secondary": "#ffffff",
                "tertiary": "#686000",
                "on-secondary-fixed-variant": "#673d00",
                "on-error-container": "#93000a",
                "secondary-fixed-dim": "#ffb866",
                "surface-container-low": "#f4f4ef",
                "surface-container-highest": "#e3e3de",
                "surface-container": "#eeeee9",
                "tertiary-fixed": "#f3e56c",
                "on-secondary-container": "#6d4100",
                "outline": "#6f7880",
                "on-tertiary-fixed": "#1f1c00",
                "on-secondary-fixed": "#2b1700",
                "on-tertiary-container": "#474100",
                "primary-container": "#4fb6e9",
                "primary-fixed": "#c4e7ff",
                "on-error": "#ffffff",
                "on-surface": "#1a1c19",
                "surface-variant": "#e3e3de",
                "on-tertiary": "#ffffff",
                "surface-tint": "#00668a",
                "background": "#fafaf5",
                "surface-container-lowest": "#ffffff",
                "inverse-on-surface": "#f1f1ec",
                "error": "#ba1a1a"
            },
            borderRadius: {
                "DEFAULT": "1rem",
                "lg": "2rem",
                "xl": "3rem",
                "full": "9999px"
            },
            fontFamily: {
                "display": ["'Baloo Bhaijaan 2'", "sans-serif"],
                "body": ["'Cairo'", "sans-serif"],
            }
          },
        },
      }
    </script>
<style>
        body {
            font-family: 'Cairo', sans-serif;
            background-color: #fafaf5; /* surface */
            color: #1a1c19; /* on-surface */
        }
        h1, h2, h3, h4, .font-display {
            font-family: 'Baloo Bhaijaan 2', sans-serif;
        }
        /* Hide scrollbar for clean UI */
        .no-scrollbar::-webkit-scrollbar {
            display: none;
        }
        .no-scrollbar {
            -ms-overflow-style: none;  /* IE and Edge */
            scrollbar-width: none;  /* Firefox */
        }
    </style>
</head>
<body class="bg-surface text-on-surface antialiased min-h-screen flex selection:bg-primary-container selection:text-on-primary-container">
<!-- Mobile Top NavBar (hidden on desktop) -->
<nav class="md:hidden fixed top-0 left-0 right-0 z-30 flex justify-between items-center px-6 h-20 bg-surface/80 backdrop-blur-xl border-none shadow-[0_4px_24px_rgba(0,0,0,0.02)]">
<div class="font-display text-2xl font-black text-primary"><img alt="نديم" class="h-12 w-auto object-contain" src="https://lh3.googleusercontent.com/aida/ADBb0uhb3K05xBavCVb8YCsNHNxC0dDQA0iQEA1KfFM9iICU-JcTNcetpLmTTfmqcDUgR8rweEGZL_FhQHSM5syLac5m6Ur3xoSv8bP8jNyNT5PG4r0SHMsHsHp8fh68Nu-ELeKvlmkBkY7adaDzAwYGW4VhS2ExRn38Brsa2cyqIVEo7Yt6ri4Hczz1BjVyJDShwPyhot3-F3RQgkecwwh9sflPZgczN1ZN4ASR6uCQWS4RUe4my1mkf3uxlZ-wNyheZLhPixhadMOUf4c"/></div>
<div class="flex gap-4">
<button class="text-on-surface-variant hover:opacity-80 transition-opacity">
<span class="material-symbols-outlined">notifications</span>
</button>
<button class="text-on-surface-variant hover:opacity-80 transition-opacity">
<span class="material-symbols-outlined">child_care</span>
</button>
</div>
</nav>
<!-- Desktop Side NavBar -->
<aside class="hidden md:flex fixed right-0 top-0 h-full w-72 z-40 flex-col p-6 space-y-8 bg-surface-container-low shadow-[-4px_0_24px_rgba(0,0,0,0.02)] rounded-l-xl">
<!-- Brand Header -->
<div class="flex flex-col items-center mt-4 mb-8">
<img alt="نديم" class="w-40 h-auto object-contain" src="https://lh3.googleusercontent.com/aida/ADBb0uhb3K05xBavCVb8YCsNHNxC0dDQA0iQEA1KfFM9iICU-JcTNcetpLmTTfmqcDUgR8rweEGZL_FhQHSM5syLac5m6Ur3xoSv8bP8jNyNT5PG4r0SHMsHsHp8fh68Nu-ELeKvlmkBkY7adaDzAwYGW4VhS2ExRn38Brsa2cyqIVEo7Yt6ri4Hczz1BjVyJDShwPyhot3-F3RQgkecwwh9sflPZgczN1ZN4ASR6uCQWS4RUe4my1mkf3uxlZ-wNyheZLhPixhadMOUf4c"/>
<p class="font-body text-sm font-medium tracking-wide text-on-surface-variant mt-2">عالم من الخيال</p>
</div>
<!-- Navigation Links -->
<nav class="flex-1 space-y-2">
<!-- Inactive -->
<a class="flex items-center px-4 py-3 rounded-full text-on-surface-variant hover:bg-surface-container-high transition-all duration-300 font-body text-sm font-medium" href="#">
<span class="material-symbols-outlined ml-4">home</span>
                الرئيسية
            </a>
<!-- Inactive -->
<a class="flex items-center px-4 py-3 rounded-full text-on-surface-variant hover:bg-surface-container-high transition-all duration-300 font-body text-sm font-medium" href="#">
<span class="material-symbols-outlined ml-4">devices</span>
                أجهزتي
            </a>
<!-- Inactive -->
<a class="flex items-center px-4 py-3 rounded-full text-on-surface-variant hover:bg-surface-container-high transition-all duration-300 font-body text-sm font-medium" href="#">
<span class="material-symbols-outlined ml-4">library_music</span>
                مكتبة السمسم
            </a>
<!-- ACTIVE TAB -->
<a class="flex items-center px-4 py-3 rounded-full bg-gradient-to-br from-primary to-primary-container text-on-primary shadow-[0_8px_16px_rgba(0,102,138,0.2)] scale-105 transition-transform ease-out font-body text-sm font-medium" href="#">
<span class="material-symbols-outlined ml-4" style="font-variation-settings: 'FILL' 1;">forum</span>
                سجل المحادثات الذكية
            </a>
<!-- Inactive -->
<a class="flex items-center px-4 py-3 rounded-full text-on-surface-variant hover:bg-surface-container-high transition-all duration-300 font-body text-sm font-medium" href="#">
<span class="material-symbols-outlined ml-4">settings</span>
                الإعدادات
            </a>
</nav>
</aside>
<!-- Main Content Canvas -->
<main class="flex-1 flex flex-col md:mr-72 pt-24 md:pt-8 px-4 md:px-12 pb-12 max-w-7xl mx-auto w-full gap-8">
<!-- Page Header -->
<header class="mb-4">
<h1 class="font-display text-4xl md:text-5xl font-bold text-primary tracking-wide">سجل المحادثات الذكية</h1>
<p class="font-body text-lg text-on-surface-variant mt-2">تابع رحلة استكشاف طفلك مع سمسم.</p>
</header>
<!-- Bento Grid Layout -->
<div class="grid grid-cols-1 lg:grid-cols-12 gap-6 lg:gap-8 flex-1 h-[716px] min-h-[600px]">
<!-- Right Column: History List (3 cols) -->
<div class="lg:col-span-4 bg-surface-container-low rounded-xl p-4 flex flex-col h-full overflow-hidden">
<div class="flex justify-between items-center mb-6 px-2">
<h2 class="font-display text-2xl font-semibold text-on-surface">الجلسات الأخيرة</h2>
<button class="text-primary hover:bg-primary/10 p-2 rounded-full transition-colors">
<span class="material-symbols-outlined">filter_list</span>
</button>
</div>
<div class="flex-1 overflow-y-auto no-scrollbar space-y-3 pr-2">
<!-- Active Session Item -->
<div class="bg-surface-container-highest p-4 rounded-lg cursor-pointer transform transition-transform hover:scale-[1.02]">
<div class="flex justify-between items-start mb-2">
<h3 class="font-display text-lg font-semibold text-primary">مغامرة في الفضاء</h3>
<span class="font-body text-xs text-on-surface-variant bg-surface px-2 py-1 rounded-full">اليوم، 4:30 م</span>
</div>
<p class="font-body text-sm text-on-surface-variant line-clamp-2">تحدث سمسم عن الكواكب والنجوم وكيفية السفر إلى المريخ باستخدام صاروخ خيالي...</p>
</div>
<!-- Inactive Session Item 1 -->
<div class="bg-surface p-4 rounded-lg cursor-pointer transform transition-transform hover:scale-[1.02] hover:bg-surface-container-highest/50">
<div class="flex justify-between items-start mb-2">
<h3 class="font-display text-lg font-semibold text-on-surface">قصة الديناصور الصغير</h3>
<span class="font-body text-xs text-on-surface-variant">أمس، 7:15 م</span>
</div>
<p class="font-body text-sm text-on-surface-variant line-clamp-2">قصة ما قبل النوم عن ديناصور يبحث عن أصدقاء في الغابة الكبيرة...</p>
</div>
<!-- Inactive Session Item 2 -->
<div class="bg-surface p-4 rounded-lg cursor-pointer transform transition-transform hover:scale-[1.02] hover:bg-surface-container-highest/50">
<div class="flex justify-between items-start mb-2">
<h3 class="font-display text-lg font-semibold text-on-surface">لعبة الألوان</h3>
<span class="font-body text-xs text-on-surface-variant">٢٢ أكتوبر</span>
</div>
<p class="font-body text-sm text-on-surface-variant line-clamp-2">تفاعل تعليمي لتعلم الألوان الأساسية من خلال أغنية ممتعة وتحديات بسيطة.</p>
</div>
<!-- Inactive Session Item 3 -->
<div class="bg-surface p-4 rounded-lg cursor-pointer transform transition-transform hover:scale-[1.02] hover:bg-surface-container-highest/50">
<div class="flex justify-between items-start mb-2">
<h3 class="font-display text-lg font-semibold text-on-surface">لماذا السماء زرقاء؟</h3>
<span class="font-body text-xs text-on-surface-variant">٢٠ أكتوبر</span>
</div>
<p class="font-body text-sm text-on-surface-variant line-clamp-2">إجابة مبسطة لأسئلة فضولية حول الطبيعة والضوء.</p>
</div>
</div>
</div>
<!-- Left Column: Active Chat Transcript (8 cols) -->
<div class="lg:col-span-8 bg-surface-container-lowest rounded-xl flex flex-col h-full relative overflow-hidden">
<!-- Chat Header -->
<div class="bg-surface-container-highest/30 px-8 py-5 flex items-center gap-4 z-10">
<div class="w-12 h-12 rounded-full bg-gradient-to-br from-primary to-primary-container flex items-center justify-center shadow-sm">
<span class="material-symbols-outlined text-white" style="font-variation-settings: 'FILL' 1;">robot_2</span>
</div>
<div>
<h2 class="font-display text-xl font-bold text-on-surface">مغامرة في الفضاء</h2>
<p class="font-body text-sm text-primary">مع سمسم • اليوم، 4:30 مساءً</p>
</div>
</div>
<!-- Chat Messages Area -->
<div class="flex-1 overflow-y-auto p-8 space-y-8 bg-[url('data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iNDAiIGhlaWdodD0iNDAiIHhtbG5zPSJodHRwOi8vd3d3LnczLm9yZy8yMDAwL3N2ZyI+PGNpcmNsZSBjeD0iMjAiIGN5PSIyMCIgcj0iMSIgZmlsbD0iIzAwNjY4YSIgZmlsbC1vcGFjaXR5PSIwLjA1Ii8+PC9zdmc+')]">
<!-- Timestamp Divider -->
<div class="flex justify-center">
<span class="font-body text-xs font-medium text-on-surface-variant bg-surface-container-low px-4 py-1.5 rounded-full">بدأت المحادثة 4:30 م</span>
</div>
<!-- AI Message (Semsem) - Left Aligned -->
<div class="flex gap-4 w-full md:w-4/5 justify-end flex-row-reverse float-left clear-both">
<div class="w-10 h-10 rounded-full bg-gradient-to-br from-primary to-primary-container flex-shrink-0 flex items-center justify-center mt-2">
<span class="material-symbols-outlined text-white text-lg">robot_2</span>
</div>
<div class="bg-surface-container-low rounded-2xl rounded-tl-sm p-5 relative group">
<p class="font-body text-base text-on-surface leading-relaxed">
                                مرحباً يا بطل! هل أنت مستعد لرحلتنا إلى الفضاء اليوم؟ لقد جهزت صاروخنا السحري. إلى أي كوكب تريد أن نسافر أولاً؟
                            </p>
<!-- Audio Play Button (Glassmorphism) -->
<button class="absolute -bottom-4 -left-4 bg-surface-container-lowest/80 backdrop-blur-md p-2.5 rounded-full text-primary shadow-[0_4px_12px_rgba(0,102,138,0.1)] hover:scale-110 transition-transform">
<span class="material-symbols-outlined text-xl" style="font-variation-settings: 'FILL' 1;">play_arrow</span>
</button>
</div>
</div>
<!-- Child Message - Right Aligned -->
<div class="flex gap-4 w-full md:w-4/5 float-right clear-both">
<div class="w-10 h-10 rounded-full bg-secondary-container flex-shrink-0 flex items-center justify-center mt-2">
<span class="material-symbols-outlined text-on-secondary-container text-lg">face</span>
</div>
<div class="bg-secondary-container rounded-2xl rounded-tr-sm p-5">
<p class="font-body text-base text-on-secondary-container leading-relaxed">
                                أريد الذهاب إلى الكوكب الأحمر الكبير! هل هناك كائنات فضائية هناك يا سمسم؟
                            </p>
</div>
</div>
<!-- AI Message (Semsem) - Left Aligned -->
<div class="flex gap-4 w-full md:w-4/5 justify-end flex-row-reverse float-left clear-both mt-12">
<div class="w-10 h-10 rounded-full bg-gradient-to-br from-primary to-primary-container flex-shrink-0 flex items-center justify-center mt-2">
<span class="material-symbols-outlined text-white text-lg">robot_2</span>
</div>
<div class="bg-surface-container-low rounded-2xl rounded-tl-sm p-5 relative group">
<p class="font-body text-base text-on-surface leading-relaxed">
                                تقصد المريخ! اختيار رائع. المريخ يسمى الكوكب الأحمر بسبب الغبار الذي يغطيه. حتى الآن، لم نجد كائنات فضائية هناك، لكن العلماء يبحثون دائماً! اربط حزام الأمان، نحن ننطلق... 🚀
                            </p>
<!-- Audio Play Button -->
<button class="absolute -bottom-4 -left-4 bg-surface-container-lowest/80 backdrop-blur-md p-2.5 rounded-full text-primary shadow-[0_4px_12px_rgba(0,102,138,0.1)] hover:scale-110 transition-transform">
<span class="material-symbols-outlined text-xl" style="font-variation-settings: 'FILL' 1;">play_arrow</span>
</button>
</div>
</div>
</div>
<!-- Bottom fade out for scroll -->
<div class="h-12 bg-gradient-to-t from-surface-container-lowest to-transparent absolute bottom-0 left-0 right-0 pointer-events-none rounded-b-xl"></div>
</div>
</div>
</main>
</body></html>
