<?php
/**
 * Weixin config form
 */
function _weixin_config_form($form, &$form_state) {
  $form['kuri_weixin_token'] = array(
    '#title' => t('Weixn Token'),
    '#type' => 'textfield',
    '#required' => TRUE,
    '#default_value' => variable_get('kuri_weixin_token', NULL),
  );
  $form['kuri_weixin_appid'] = array(
    '#title' => t('Weixn appID'),
    '#type' => 'textfield',
    '#default_value' => variable_get('kuri_weixin_appid', NULL),
  );
  $form['kuri_weixin_appsecret'] = array(
    '#title' => t('Weixn appSecret'),
    '#type' => 'textfield',
    '#default_value' => variable_get('kuri_weixin_appsecret', NULL),
  );
  $form['kuri_weixin_debug'] = array(
    '#title' => t('Debug Mode ?'),
    '#type' => 'checkbox',
    '#default_value' => variable_get('kuri_weixin_debug', 0),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save Settings'),
  );
  return $form;
}

/**
 * Save weixin config
 */
function _weixin_config_form_submit($form, &$form_state) {
  variable_set('kuri_weixin_token', $form_state['values']['kuri_weixin_token']);
  variable_set('kuri_weixin_appid', $form_state['values']['kuri_weixin_appid']);
  variable_set('kuri_weixin_appsecret', $form_state['values']['kuri_weixin_appsecret']);
  variable_set('kuri_weixin_debug', $form_state['values']['kuri_weixin_debug']);

  drupal_set_message(t('Your configuration have been saved.'));
}

