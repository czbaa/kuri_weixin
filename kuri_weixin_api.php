<?php
/**
 * Weixin API Class
 *
 * Check Weixin Wiki http://mp.weixin.qq.com/wiki/index.php
 *
 * By Eric.Lee <ericstone.dev@gmail.com>
 */
class kuriWeixinAPI {

  private $_token;
  private $_appid;
  private $_appsecret;
  private $_applink = 'https://api.weixin.qq.com/cgi-bin/token?grant_type=client_credential&appid=%s&secret=%s';

  // body of reply message
  public $xmlReplyText = <<<XML
<xml>
  <ToUserName><![CDATA[%s]]></ToUserName>
  <FromUserName><![CDATA[%s]]></FromUserName>
  <CreateTime>%s</CreateTime>
  <MsgType><![CDATA[%s]]></MsgType>
  <Content><![CDATA[%s]]></Content>
</xml>
XML;
  public $xmlReplyNewsArticle = <<<XML
<item>
  <Title><![CDATA[%s]]></Title>
  <Description><![CDATA[%s]]></Description>
  <PicUrl><![CDATA[%s]]></PicUrl>
  <Url><![CDATA[%s]]></Url>
</item>
XML;
  public $xmlReplyNews = <<<XML
<xml>
  <ToUserName><![CDATA[%s]]></ToUserName>
  <FromUserName><![CDATA[%s]]></FromUserName>
  <CreateTime>%s</CreateTime>
  <MsgType><![CDATA[%s]]></MsgType>
  <ArticleCount>%s</ArticleCount>
  <Articles>
  %s
  </Articles>
</xml>
XML;
  public $xmlReplyMusic = <<<XML
<xml>
  <ToUserName><![CDATA[%s]]></ToUserName>
  <FromUserName><![CDATA[%s]]></FromUserName>
  <CreateTime>%s</CreateTime>
  <MsgType><![CDATA[%s]]></MsgType>
  <Music>
  <Title><![CDATA[%s]]></Title>
  <Description><![CDATA[%s]]></Description>
  <MusicUrl><![CDATA[%s]]></MusicUrl>
  <HQMusicUrl><![CDATA[%s]]></HQMusicUrl>
  </Music>
</xml>
XML;

  public function __construct($token = '', $appid = '', $appsecret = '') {
    $this->_token = $token;
    $this->_appid = $appid;
    $this->_appsecret = $appsecret;
  }

  public function valid() {
    $echoStr = $_GET["echostr"];

    //valid signature , option
    if($this->checkSignature()){
      echo $echoStr;
      exit;
    }
  }

  private function checkSignature() {
    $signature = $_GET["signature"];
    $timestamp = $_GET["timestamp"];
    $nonce = $_GET["nonce"];

    $tmpArr = array($this->_token, $timestamp, $nonce);
    sort($tmpArr, SORT_STRING);
    $tmpStr = implode( $tmpArr );
    $tmpStr = sha1( $tmpStr );

    if( $tmpStr == $signature ){
      return TRUE;
    }else{
      return FALSE;
    }
  }

  public function responseMsg() {
    //get post data, May be due to the different environments
    $postStr = $GLOBALS["HTTP_RAW_POST_DATA"];

    //extract post data
    if (!empty($postStr)){
      $postObj = simplexml_load_string($postStr, 'SimpleXMLElement', LIBXML_NOCDATA);
      return $postObj;
    } else {
      return FALSE;
    }
  }

  public function getAccessToken() {
    $url = sprintf($this->_applink, $this->_appid, $this->_appsecret);
    $data = file_get_contents($url);
    $data = json_decode($data);
    if (isset($data->access_token)) return $data->access_token;
    return '';
  }

  public function replyText(weixinReplyTextObject $reply) {
    $output = sprintf($this->xmlReplyText, $reply->toUserName, $reply->fromUserName, $reply->createTime, $reply->msgType, $reply->content);
    print $output;
  }

  public function replyNews(weixinReplyNewsObject $reply) {
    $articles = '';
    foreach ($reply->articles as $article) {
      $articles .= sprintf($this->xmlReplyNewsArticle, $article->title, $article->description, $article->picUrl, $article->url);
    }

    $reply->articleCount = count($reply->articles);
    $output = sprintf($this->xmlReplyNews, $reply->toUserName, $reply->fromUserName, $reply->createTime, $reply->msgType, $reply->articleCount, $articles);
    print $output;
  }

  public function replyMusic(weixinReplyMusicObject $reply) {
    $output = sprintf($this->xmlReplyMusic, $reply->toUserName, $reply->fromUserName, $reply->createTime, $reply->msgType, $reply->title, $reply->description, $reply->musicUrl, $reply->hdMusicUrl);
    print $output;
  }

  public function pushMenu($menu_json) {
    $accessToken = $this->getAccessToken();
    if ($accessToken) {
      $url = "https://api.weixin.qq.com/cgi-bin/menu/create?access_token=" . $accessToken;

      // post data to weixin
      $ch = curl_init();
      curl_setopt($ch, CURLOPT_URL, $url);
      curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
      curl_setopt($ch, CURLOPT_POST, 1);
      curl_setopt($ch, CURLOPT_HTTPHEADER, array("Content-Type: application/json; charset=utf-8"));
      curl_setopt($ch, CURLOPT_POSTFIELDS, $menu_json);
      $data = curl_exec($ch);
      curl_close($ch);

      $data = json_decode($data);
      if ($data->errcode == 0) {
        drupal_set_message('Custom menu push to Weixin success!!! Check your device.');
      } else {
        drupal_set_message('Custom menu push to Weixin failure. ' . $data->errcode . ' ' . $data->errmsg, 'error');
      }
    } else {
      drupal_set_message('Exchange access token from weixin failure', 'error');
    }
  }
}

class weixinReplyObject {
  public $toUserName;
  public $fromUserName;
  public $createTime;
  public $msgType;

  public function __construct() {
    $this->createTime = time();
  }
}

class weixinReplyArticleObject {
  public $title;
  public $description;
  public $picUrl;
  public $url;
}

class weixinReplyTextObject extends weixinReplyObject {
  public $content;

  public function __construct() {
    parent::__construct();
    $this->msgType = 'text';
  }
}


class weixinReplyNewsObject extends weixinReplyObject {
  public $articleCount;
  public $articles = array();

  public function __construct() {
    parent::__construct();
    $this->msgType = 'news';
  }
}

class weixinReplyMusicObject extends weixinReplyObject {
  public $title;
  public $description;
  public $musicUrl;
  public $hdMusicUrl;

  public function __construct() {
    parent::__construct();
    $this->msgType = 'music';
  }
}

