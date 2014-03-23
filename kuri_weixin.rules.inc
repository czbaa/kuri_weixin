<?php
/**
 * Implements of hook_rules_condition_info()
 */
function kuri_weixin_rules_condition_info() {
  $conditions = array(
    'weixin_reply_exist' => array(
      'group' => t('Weixin'),
      'label' => t('Weixin Reply Exist'),
      'parameter' => array(
        'value' => array(
          'type' => 'text',
          'label' => t('Trigger Keyword'),
          'description' => t('The trigger keyword that Weixin reply to be fetched.'),
        ),
      ),
      'base' => 'rules_condition_weixin_reply_exist',
    ),
  );
  return $conditions;
}
/**
 * hook_rules_action_info()
 */
function kuri_weixin_rules_action_info() {
  $actions = array(
    'weixin_reply_generate' => array(
      'group' => t('Weixin'),
      'label' => t('Weixin Reply Generate'),
      'parameter' => array(
        'value' => array(
          'type' => 'text',
          'label' => t('Trigger Keyword'),
          'description' => t('The trigger keyword that Weixin reply to be fetched.'),
        ),
        'weixin_received' => array(
          'type' => 'node',
          'label' => t('Weixin Received Msg'),
        ),
      ),
      'provides' => array(
        'weixin_reply_fetched' => array('type' => 'unknown', 'label' => t('Fetched Weixin Reply')),
      ),
      'base' => 'rules_action_weixin_reply_generate',
    ),
    'weixin_reply_default_msg' => array(
      'group' => t('Weixin'),
      'label' => t('Weixin Reply Default Message'),
      'parameter' => array(
        'value' => array(
          'type' => 'text',
          'label' => t('Trigger Keyword'),
          'description' => t('The trigger keyword that Weixin reply to be fetched.'),
        ),
        'weixin_received' => array(
          'type' => 'node',
          'label' => t('Weixin Received Msg'),
        ),
      ),
      'provides' => array(
        'weixin_reply_default_fetched' => array('type' => 'unknown', 'label' => t('Fetched Weixin Reply')),
      ),
      'base' => 'rules_action_weixin_reply_default_msg',
    ),
  );

  return $actions;
}

function rules_condition_weixin_reply_exist($keyword) {
  $result = _retrieve_reply_data($keyword);
  return !empty($result);
}


function rules_action_weixin_reply_generate($keyword, $received) {
  module_load_include('inc', 'kuri_weixin', 'kuri_weixin_api');
  $kuriWeixinAPI = new kuriWeixinAPI();

  $fromUserName = $received->field_weixin_tousername[LANGUAGE_NONE][0]['value'];
  $toUserName = $received->field_weixin_fromusername_plain[LANGUAGE_NONE][0]['value'];

  $result = _retrieve_reply_data($keyword);
  $entities = entity_load('node', array_keys($result['node']));
  $node = array_shift($entities);

  switch ($node->type) {
    case 'weixin_reply_text':
      $replyText = new weixinReplyTextObject();
      $replyText->fromUserName = $fromUserName;
      $replyText->toUserName = $toUserName;
      $replyText->content = $node->field_weixin_content[LANGUAGE_NONE][0]['value'];
      $kuriWeixinAPI->replyText($replyText);
      break;
    case 'weixin_reply_news':
      $replyNews = new weixinReplyNewsObject();
      $replyNews->fromUserName = $fromUserName;
      $replyNews->toUserName = $toUserName;

      foreach ($node->field_weixin_reply_news[LANGUAGE_NONE] as $key => $item) {
        $entity = field_collection_field_get_entity($item);
        $article = new weixinReplyArticleObject();
        $article->title = $entity->field_weixin_reply_news_title[LANGUAGE_NONE][0]['value'];
        $article->description = $entity->field_weixin_reply_news_desc[LANGUAGE_NONE][0]['value'];
        $article->url = $entity->field_weixin_reply_news_url[LANGUAGE_NONE][0]['url'];
        $article->picUrl = file_create_url($entity->field_weixin_reply_news_pic[LANGUAGE_NONE][0]['uri']);
        $replyNews->articles[] = $article;
      }
      $kuriWeixinAPI->replyNews($replyNews);
      break;
    case 'weixin_reply_music':
      $replyMusic = new weixinReplyMusicObject();
      $replyMusic->fromUserName = $fromUserName;
      $replyMusic->toUserName = $toUserName;
      $replyMusic->title = $node->field_weixin_reply_music_title[LANGUAGE_NONE][0]['value'];
      $replyMusic->description = $node->field_weixin_reply_music_desc[LANGUAGE_NONE][0]['value'];
      $replyMusic->musicUrl = $replyMusic->hdMusicUrl = file_create_url($node->field_weixin_reply_music_file[LANGUAGE_NONE][0]['uri']);
      $kuriWeixinAPI->replyMusic($replyMusic);
      break;
  }
  exit;
}

/**
 * The Default Reply Message, which means none of trigger keywords matched in system
 * then use by trigger keyword: default-msg-do-not-delete, this is hardcode.
 */
function rules_action_weixin_reply_default_msg($keyword, $received) {
  rules_action_weixin_reply_generate($keyword, $received);
}


function _retrieve_reply_data($keyword) {
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'node')
    ->entityCondition('bundle', array('weixin_reply_text', 'weixin_reply_news', 'weixin_reply_music'), 'IN')
    ->propertyCondition('title', $keyword)
    ->propertyCondition('status', 1)
    ->range(0, 1);
  return $query->execute();
}

