<?php

/**
 * Renders the content using the jQuery UI Accordion widget.
 */
class QuickAccordion extends QuickRenderer {

  public static function optionsForm($qt) {
    $form = array();
    $form['history'] = array(
      '#type' => 'checkbox',
      '#title' => 'History',
      '#description' => t('Store tab state in the URL allowing for browser back / forward and bookmarks.'),
      '#default_value' => (isset($qt->renderer) && $qt->renderer == 'accordion' && isset($qt->options['history']) && $qt->options['history']),
    );
    $form['jquery_ui'] = array(
      '#type' => 'fieldset',
      '#title' => t('JQuery UI options'),
    );
    $form['jquery_ui']['autoHeight'] = array(
      '#type' => 'checkbox',
      '#title' => 'Autoheight',
      '#default_value' => (isset($qt->renderer) && $qt->renderer == 'accordion' && isset($qt->options['jquery_ui']['autoHeight']) && $qt->options['jquery_ui']['autoHeight']),
    );
    $form['jquery_ui']['collapsible'] = array(
      '#type' => 'checkbox',
      '#title' => t('Collapsible'),
      '#default_value' => (isset($qt->renderer) && $qt->renderer == 'accordion' && isset($qt->options['jquery_ui']['collapsible']) && $qt->options['jquery_ui']['collapsible']),
    );
    return $form;
  }

  public function render() {
    $quickset = $this->quickset;
    $qsid = 'quickset-' . $quickset->getName();
    // Build our render array...
    $render_array = array();
    $render_array['#attached'] = $this->add_attached();
    $render_array['content'] = array(
      '#theme' => 'qt_accordion',
      '#options' => array('attributes' => array(
        'id' => $qsid,
        'class' => array('quick-accordion'),
      )),
      'divs' => array(),
    );

    // Render all tab content.
    foreach ($quickset->getContents() as $key => $item) {
      if (!empty($item)) {
        $attributes = array();
        $attributes['class'][] = backdrop_html_class($item->getTitle());
        backdrop_alter('quicktabs_tablinks_attributes', $attributes, $quickset, $key);
        $attributes['href'] = '#'. $qsid . '_' . $key ;
        $render_array['content']['divs'][] = array(
          '#prefix' => '<h3><a '. backdrop_attributes($attributes) .'>'. check_plain($quickset->translateString($item->getTitle(), 'tab', $key)) .'</a></h3><div>',
          '#suffix' => '</div>',
          'content' => $item->render(),
        );
      }
    }
    return $render_array;
  }

  /**
   * Add any necessary js, css and libraries for the render array.
   */
  protected function add_attached() {
    $settings = $this->quickset->getSettings();
    // Setting heightStyle option so accordions don't auto height with newer versions of jQuery UI
    if (!isset($settings['options']['jquery_ui']['autoheight'])) {
      $settings['options']['jquery_ui']['heightStyle'] = 'content';
    }
    $options = $settings['options'];
    $attached = array(
      'library' => array(
        array('quicktabs', 'quicktabs.accordion'),
      ),
    );

    $javascript = backdrop_add_js();
    if (isset($javascript['settings']['data'])) {
      foreach ($javascript['settings']['data'] as $key => $settings) {
        if (key($settings) == 'quicktabs') {
          $qtkey = $key;
          break;
        }
      }
    }

    if ($options['history']) {
      $attached['library'][] = array('quicktabs', 'quicktabs.bbq');
    }

    $name = $this->quickset->getName();
    if (!isset($qtkey) || !array_key_exists('qt_' . $name, $javascript['settings']['data'][$qtkey]['quicktabs'])) {
      $quicktabs_array = array('name' => $name, 'active_tab' => $this->quickset->getActiveTab(), 'options' => $options['jquery_ui'], 'history' => $options['history']);
      $attached['js'][] = array('data' => array('quicktabs' => array('qt_'. $name => $quicktabs_array)), 'type' => 'setting');
    }
    return $attached;
  }

}
