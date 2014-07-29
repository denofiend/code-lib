<?php
/**
 * The base configurations of the WordPress.
 *
 * This file has the following configurations: MySQL settings, Table Prefix,
 * Secret Keys, WordPress Language, and ABSPATH. You can find more information
 * by visiting {@link http://codex.wordpress.org/Editing_wp-config.php Editing
 * wp-config.php} Codex page. You can get the MySQL settings from your web host.
 *
 * This file is used by the wp-config.php creation script during the
 * installation. You don't have to use the web site, you can just copy this file
 * to "wp-config.php" and fill in the values.
 *
 * @package WordPress
 */

// ** MySQL settings - You can get this info from your web host ** //
/** The name of the database for WordPress */
define('DB_NAME', 'test');

/** MySQL database username */
define('DB_USER', 'test');

/** MySQL database password */
define('DB_PASSWORD', 'test');

/** MySQL hostname */
define('DB_HOST', '127.0.0.1');

/** Database Charset to use in creating database tables. */
define('DB_CHARSET', 'utf8');

/** The Database Collate type. Don't change this if in doubt. */
define('DB_COLLATE', '');

/**#@+
 * Authentication Unique Keys and Salts.
 *
 * Change these to different unique phrases!
 * You can generate these using the {@link https://api.wordpress.org/secret-key/1.1/salt/ WordPress.org secret-key service}
 * You can change these at any point in time to invalidate all existing cookies. This will force all users to have to log in again.
 *
 * @since 2.6.0
 */
define('AUTH_KEY',         'G2@*z}#Vgi!OU4 >S,<Gl3c9r-I_df ~7d@g9Q9V8Y9e<f>0$/UH/y-Zu7Sshe:M');
define('SECURE_AUTH_KEY',  'Xvyuks#T~+8zf?Z@HD2&r33H<R(7Lxo9lBn#b!rU@Fm%VlY3?^+>%DE^eCxTIPQt');
define('LOGGED_IN_KEY',    'tZ=FDu.-q,5AqR#|:9wwK[#/&zIb9hWw&|Sa%T#;xf/kH/<89]!W=Ul6KSJ~0)dY');
define('NONCE_KEY',        '!<,X_wDw*A8tBE?q3V8H*FTcOdN`:>^*dUj#Fix$Lv{xB}k9qwfx.DO(RRu{Ah6H');
define('AUTH_SALT',        '^5Go2^cUZ^bHHfTj5$Te$8hF&i3h :R-Vk.;+2ndS<=?n<)d88.Z4#-/^10bVb-h');
define('SECURE_AUTH_SALT', 'GggLL8y~z .I*:_0?W[Bg-.J*oDBE}|tGml^oJ7~qE*RvonGl_c9I,r}8x6.|wp.');
define('LOGGED_IN_SALT',   'VA.);Sb$YYvSZb9A;bndID97IS?)w(YJ~7Zx!,nGI2nO_!<muK@M<^$3va0c _By');
define('NONCE_SALT',       'T{jE12dxW+uz;t4OH,LoGR5O+A%Rsd&xJ:xeRlX*],9AAJaAPp_9$6guL9@)eJ?z');

/**#@-*/

/**
 * WordPress Database Table prefix.
 *
 * You can have multiple installations in one database if you give each a unique
 * prefix. Only numbers, letters, and underscores please!
 */
$table_prefix  = 'wp_';

/**
 * WordPress Localized Language, defaults to English.
 *
 * Change this to localize WordPress. A corresponding MO file for the chosen
 * language must be installed to wp-content/languages. For example, install
 * de_DE.mo to wp-content/languages and set WPLANG to 'de_DE' to enable German
 * language support.
 */
define('WPLANG', 'zh_CN');

/**
 * For developers: WordPress debugging mode.
 *
 * Change this to true to enable the display of notices during development.
 * It is strongly recommended that plugin and theme developers use WP_DEBUG
 * in their development environments.
 */
define('WP_DEBUG', false);

/* That's all, stop editing! Happy blogging. */

/** Absolute path to the WordPress directory. */
if ( !defined('ABSPATH') )
	define('ABSPATH', dirname(__FILE__) . '/');

/** Sets up WordPress vars and included files. */
require_once(ABSPATH . 'wp-settings.php');
